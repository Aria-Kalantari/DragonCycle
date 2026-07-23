#include "DragonRoundSummary.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"

// ---------------------------------------------------------------------------
// FDragonRoundSummary
// ---------------------------------------------------------------------------

FDragonFighterRoundStats& FDragonRoundSummary::StatsFor(FName Fighter)
{
    return FighterStats.FindOrAdd(Fighter);
}

void FDragonRoundSummary::BeginRound(FName InEncounterId, int32 InRoundId, FName InArenaId,
                                     const FDragonFighterProfile& InA, const FDragonFighterProfile& InB,
                                     double InStartTime)
{
    *this = FDragonRoundSummary();
    EncounterId = InEncounterId;
    RoundId = InRoundId;
    ArenaId = InArenaId;
    FighterA = InA;
    FighterB = InB;
    StartTime = InStartTime;

    // Seed the stat maps so a fighter that never acts still reports zeroed rows.
    StatsFor(InA.Id);
    StatsFor(InB.Id);
}

void FDragonRoundSummary::Finalize(FName InWinner, EDragonWinCondition InWinCondition, double InEndTime)
{
    if (!ensureMsgf(DragonTelemetry::IsValidWinCondition(static_cast<uint8>(InWinCondition)),
                    TEXT("Finalize rejected an out-of-range win condition (%d)."),
                    static_cast<int32>(InWinCondition)))
    {
        return;
    }

    Winner = InWinner;
    WinCondition = InWinCondition;
    EndTime = InEndTime;
    bFinalized = true;
}

void FDragonRoundSummary::IngestEvent(const FDragonCombatEvent& Event)
{
    ++EventCount;

    // Body-part damage distribution — the one aggregate read straight off the
    // event stream, so there is a single source for it.
    float& PartDamage = DamageByPart.FindOrAdd(Event.TargetBodyPart);
    PartDamage += Event.AppliedDamage;
    TotalAppliedDamage += Event.AppliedDamage;

    if (Event.AbilityTag.IsValid())
    {
        int32& Uses = AbilityUsage.FindOrAdd(Event.AbilityTag);
        ++Uses;
    }
}

void FDragonRoundSummary::SampleOccupancy(FName Fighter, EDragonLocomotionState Locomotion, FIntPoint Cell,
                                          float DeltaSeconds, bool bInContact)
{
    if (DeltaSeconds <= 0.0f)
    {
        return;
    }

    FDragonFighterRoundStats& Stats = StatsFor(Fighter);
    switch (Locomotion)
    {
    case EDragonLocomotionState::Airborne: Stats.TimeAirborne += DeltaSeconds; break;
    case EDragonLocomotionState::Grappled: Stats.TimeGrappled += DeltaSeconds; break;
    case EDragonLocomotionState::Grounded:
    default:                               Stats.TimeGrounded += DeltaSeconds; break;
    }

    if (bInContact)
    {
        Stats.TimeInContact += DeltaSeconds;
    }

    float& CellTime = Stats.TerrainOccupancy.FindOrAdd(Cell);
    CellTime += DeltaSeconds;
}

void FDragonRoundSummary::RecordStaminaExhaustion(FName Fighter) { ++StatsFor(Fighter).StaminaExhaustionEvents; }
void FDragonRoundSummary::RecordDisengage(FName Fighter)         { ++StatsFor(Fighter).DisengageEvents; }
void FDragonRoundSummary::RecordReengage(FName Fighter)          { ++StatsFor(Fighter).ReengageEvents; }
void FDragonRoundSummary::RecordDiveAttempt(FName Fighter)       { ++StatsFor(Fighter).DiveAttempts; }

void FDragonRoundSummary::RecordDiveConversion(FName Fighter)
{
    // A conversion presupposes an attempt; count the attempt too if the caller
    // only reported the conversion, so the rate can never exceed 1.
    FDragonFighterRoundStats& Stats = StatsFor(Fighter);
    ++Stats.DiveConversions;
    if (Stats.DiveConversions > Stats.DiveAttempts)
    {
        Stats.DiveAttempts = Stats.DiveConversions;
    }
}

void FDragonRoundSummary::RecordGrappleAttempt(FName Fighter, EDragonGrappleOutcome Outcome)
{
    FDragonFighterRoundStats& Stats = StatsFor(Fighter);
    ++Stats.GrappleAttempts;
    int32& Count = Stats.GrappleOutcomes.FindOrAdd(Outcome);
    ++Count;
}

void FDragonRoundSummary::RecordAssessment(const FDragonAssessmentTracePoint& Point)
{
    AssessmentTrace.Add(Point);
}

float FDragonRoundSummary::ContactFraction(FName Fighter) const
{
    const double RoundDuration = Duration();
    if (RoundDuration <= 0.0)
    {
        return 0.0f;
    }
    const FDragonFighterRoundStats* Stats = FighterStats.Find(Fighter);
    if (!Stats)
    {
        return 0.0f;
    }
    return static_cast<float>(Stats->TimeInContact / RoundDuration);
}

// --- JSON serialization ----------------------------------------------------

namespace
{
    const TCHAR* MorphotypeToString(EDragonMorphotype Morphotype)
    {
        switch (Morphotype)
        {
        case EDragonMorphotype::Drake:    return TEXT("drake");
        case EDragonMorphotype::Wyvern:   return TEXT("wyvern");
        case EDragonMorphotype::Longneck: return TEXT("longneck");
        default:                          return TEXT("unknown");
        }
    }

    const TCHAR* GrappleOutcomeToString(EDragonGrappleOutcome Outcome)
    {
        switch (Outcome)
        {
        case EDragonGrappleOutcome::Pin:    return TEXT("pin");
        case EDragonGrappleOutcome::Throw:  return TEXT("throw");
        case EDragonGrappleOutcome::Escape: return TEXT("escape");
        case EDragonGrappleOutcome::Fall:   return TEXT("fall");
        case EDragonGrappleOutcome::Broken: return TEXT("broken");
        default:                            return TEXT("unknown");
        }
    }

    FString BodyPartToString(EDragonBodyPart Part)
    {
        // Reuse the reflected enum names so this never drifts from the closed set.
        if (const UEnum* EnumPtr = StaticEnum<EDragonBodyPart>())
        {
            return EnumPtr->GetNameStringByValue(static_cast<int64>(Part));
        }
        return FString::FromInt(static_cast<int32>(Part));
    }

    TSharedRef<FJsonObject> FighterProfileToJson(const FDragonFighterProfile& Profile)
    {
        TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
        Obj->SetStringField(TEXT("id"), Profile.Id.ToString());
        Obj->SetStringField(TEXT("morphotype"), MorphotypeToString(Profile.Morphotype));
        Obj->SetNumberField(TEXT("life_stage"), static_cast<int32>(Profile.LifeStage));
        Obj->SetNumberField(TEXT("experience"), static_cast<int32>(Profile.Experience));
        Obj->SetStringField(TEXT("difficulty_tier"), Profile.DifficultyTier.ToString());
        return Obj;
    }

    TSharedRef<FJsonObject> FighterStatsToJson(const FDragonFighterRoundStats& Stats)
    {
        TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
        Obj->SetNumberField(TEXT("time_airborne"), Stats.TimeAirborne);
        Obj->SetNumberField(TEXT("time_grounded"), Stats.TimeGrounded);
        Obj->SetNumberField(TEXT("time_grappled"), Stats.TimeGrappled);
        Obj->SetNumberField(TEXT("time_in_contact"), Stats.TimeInContact);
        Obj->SetNumberField(TEXT("stamina_exhaustion_events"), Stats.StaminaExhaustionEvents);
        Obj->SetNumberField(TEXT("disengage_events"), Stats.DisengageEvents);
        Obj->SetNumberField(TEXT("reengage_events"), Stats.ReengageEvents);
        Obj->SetNumberField(TEXT("dive_attempts"), Stats.DiveAttempts);
        Obj->SetNumberField(TEXT("dive_conversions"), Stats.DiveConversions);
        Obj->SetNumberField(TEXT("dive_conversion_rate"), Stats.DiveConversionRate());
        Obj->SetNumberField(TEXT("grapple_attempts"), Stats.GrappleAttempts);

        TSharedRef<FJsonObject> Outcomes = MakeShared<FJsonObject>();
        for (const TPair<EDragonGrappleOutcome, int32>& Pair : Stats.GrappleOutcomes)
        {
            Outcomes->SetNumberField(GrappleOutcomeToString(Pair.Key), Pair.Value);
        }
        Obj->SetObjectField(TEXT("grapple_outcomes"), Outcomes);

        // Terrain heatmap: "x,y" -> seconds. Keyed by string because JSON object
        // keys must be strings; the cell is recoverable by splitting on the comma.
        TSharedRef<FJsonObject> Heatmap = MakeShared<FJsonObject>();
        for (const TPair<FIntPoint, float>& Pair : Stats.TerrainOccupancy)
        {
            const FString Key = FString::Printf(TEXT("%d,%d"), Pair.Key.X, Pair.Key.Y);
            Heatmap->SetNumberField(Key, Pair.Value);
        }
        Obj->SetObjectField(TEXT("terrain_occupancy"), Heatmap);
        return Obj;
    }
}

TSharedRef<FJsonObject> FDragonRoundSummary::ToJson() const
{
    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();

    Root->SetStringField(TEXT("encounter_id"), EncounterId.ToString());
    Root->SetNumberField(TEXT("round_id"), RoundId);
    Root->SetStringField(TEXT("arena_id"), ArenaId.ToString());
    Root->SetObjectField(TEXT("fighter_a"), FighterProfileToJson(FighterA));
    Root->SetObjectField(TEXT("fighter_b"), FighterProfileToJson(FighterB));

    Root->SetNumberField(TEXT("start_time"), StartTime);
    Root->SetNumberField(TEXT("end_time"), EndTime);
    Root->SetNumberField(TEXT("duration"), Duration());
    Root->SetBoolField(TEXT("finalized"), bFinalized);
    Root->SetStringField(TEXT("winner"), Winner.ToString());
    Root->SetStringField(TEXT("win_condition"), DragonTelemetry::WinConditionToString(WinCondition));

    Root->SetNumberField(TEXT("event_count"), EventCount);
    Root->SetNumberField(TEXT("total_applied_damage"), TotalAppliedDamage);

    TSharedRef<FJsonObject> Fighters = MakeShared<FJsonObject>();
    for (const TPair<FName, FDragonFighterRoundStats>& Pair : FighterStats)
    {
        Fighters->SetObjectField(Pair.Key.ToString(), FighterStatsToJson(Pair.Value));
    }
    Root->SetObjectField(TEXT("fighter_stats"), Fighters);

    TSharedRef<FJsonObject> PartDamage = MakeShared<FJsonObject>();
    for (const TPair<EDragonBodyPart, float>& Pair : DamageByPart)
    {
        PartDamage->SetNumberField(BodyPartToString(Pair.Key), Pair.Value);
    }
    Root->SetObjectField(TEXT("body_part_damage"), PartDamage);

    TSharedRef<FJsonObject> Abilities = MakeShared<FJsonObject>();
    for (const TPair<FGameplayTag, int32>& Pair : AbilityUsage)
    {
        Abilities->SetNumberField(Pair.Key.ToString(), Pair.Value);
    }
    Root->SetObjectField(TEXT("ability_usage"), Abilities);

    TArray<TSharedPtr<FJsonValue>> Trace;
    Trace.Reserve(AssessmentTrace.Num());
    for (const FDragonAssessmentTracePoint& Point : AssessmentTrace)
    {
        TSharedRef<FJsonObject> PointObj = MakeShared<FJsonObject>();
        PointObj->SetNumberField(TEXT("timestamp"), Point.Timestamp);
        PointObj->SetStringField(TEXT("fighter"), Point.Fighter.ToString());
        PointObj->SetNumberField(TEXT("self_capability"), Point.SelfCapability);
        PointObj->SetNumberField(TEXT("opponent_capability"), Point.OpponentCapability);
        PointObj->SetNumberField(TEXT("accrued_cost"), Point.AccruedCost);
        PointObj->SetNumberField(TEXT("cost_threshold"), Point.CostThreshold);
        PointObj->SetBoolField(TEXT("disengage_decision"), Point.bDisengageDecision);
        Trace.Add(MakeShared<FJsonValueObject>(PointObj));
    }
    Root->SetArrayField(TEXT("assessment_trace"), Trace);

    return Root;
}

// ---------------------------------------------------------------------------
// FDragonRoundSummaryWriter
// ---------------------------------------------------------------------------

FString FDragonRoundSummaryWriter::SummaryToJsonLine(const FDragonRoundSummary& Summary)
{
    FString Line;
    // Condensed policy: one round per physical line, no pretty printing, so the
    // log is greppable and streamable.
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Line);
    FJsonSerializer::Serialize(Summary.ToJson(), Writer);
    return Line;
}

FDragonRoundSummaryWriter::~FDragonRoundSummaryWriter()
{
    Close();
}

bool FDragonRoundSummaryWriter::Open(const FString& AbsolutePath, bool bAppend)
{
    Close();

    // Make sure the directory exists; a missing Saved/Telemetry folder is the
    // common first-run failure and is cheap to fix here.
    const FString Directory = FPaths::GetPath(AbsolutePath);
    if (!Directory.IsEmpty())
    {
        IFileManager::Get().MakeDirectory(*Directory, /*Tree=*/true);
    }

    uint32 WriteFlags = FILEWRITE_AllowRead;
    if (bAppend)
    {
        WriteFlags |= FILEWRITE_Append;
    }

    Archive = IFileManager::Get().CreateFileWriter(*AbsolutePath, WriteFlags);
    if (!Archive)
    {
        return false;
    }

    Path = AbsolutePath;
    AppendedCount = 0;
    return true;
}

bool FDragonRoundSummaryWriter::Append(const FDragonRoundSummary& Summary)
{
    if (!Archive)
    {
        return false;
    }

    FString Line = SummaryToJsonLine(Summary);
    Line.AppendChar(TEXT('\n'));

    FTCHARToUTF8 Utf8(*Line);
    Archive->Serialize(const_cast<ANSICHAR*>(Utf8.Get()), Utf8.Length());
    ++AppendedCount;
    return true;
}

void FDragonRoundSummaryWriter::Flush()
{
    if (Archive)
    {
        Archive->Flush();
    }
}

void FDragonRoundSummaryWriter::Close()
{
    if (Archive)
    {
        Archive->Flush();
        Archive->Close();
        delete Archive;
        Archive = nullptr;
    }
}
