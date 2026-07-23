#pragma once

#include "CoreMinimal.h"
#include "DragonCombatTelemetry.h"
#include "DragonRoundSummary.generated.h"

class FArchive;
class FJsonObject;

/**
 * Per-fighter accumulators for one round. Every quantity here is deterministic:
 * given the same sequence of Sample/Record calls it produces the same numbers,
 * which is what makes the aggregation math testable without an engine tick.
 */
USTRUCT(BlueprintType)
struct DRAGONCYCLE_API FDragonFighterRoundStats
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    float TimeAirborne = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    float TimeGrounded = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    float TimeGrappled = 0.0f;

    /** Time spent in weapon/grapple contact range. Feeds threshold F2. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    float TimeInContact = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    int32 StaminaExhaustionEvents = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    int32 DisengageEvents = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    int32 ReengageEvents = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    int32 DiveAttempts = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    int32 DiveConversions = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    int32 GrappleAttempts = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    TMap<EDragonGrappleOutcome, int32> GrappleOutcomes;

    /** Seconds of presence per terrain cell. Not UPROPERTY: FIntPoint-keyed maps
     *  are serialized to JSON by hand, and this keeps reflection simple. */
    TMap<FIntPoint, float> TerrainOccupancy;

    /** Fraction of dive attempts that converted to a committed attack. 0 with no
     *  attempts. This is one of the deterministic aggregates under test. */
    float DiveConversionRate() const
    {
        return DiveAttempts > 0 ? static_cast<float>(DiveConversions) / static_cast<float>(DiveAttempts) : 0.0f;
    }

    float TotalTracked() const { return TimeAirborne + TimeGrounded + TimeGrappled; }
};

/**
 * Per-round telemetry aggregate (Docs/TELEMETRY.md "RoundSummary"). Holds every
 * listed aggregate: duration, winner, win condition, per-fighter time in each
 * locomotion mode, stamina-exhaustion / disengage / re-engage counts, dive
 * attempts and conversion, grapple attempts and outcome distribution, a terrain
 * occupancy heatmap, body-part damage distribution, and the assessment trace.
 *
 * It is a pure accumulator. It never decides a win condition or an impairment;
 * callers push facts in and it sums them. That separation is what lets the
 * aggregation math be unit-tested with hand-authored inputs.
 */
USTRUCT(BlueprintType)
struct DRAGONCYCLE_API FDragonRoundSummary
{
    GENERATED_BODY()

    // --- Identity ---------------------------------------------------------

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    FName EncounterId = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    int32 RoundId = 0;

    /** Arena the round was fought in. Groups occupancy heatmaps for F4. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    FName ArenaId = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    FDragonFighterProfile FighterA;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    FDragonFighterProfile FighterB;

    // --- Outcome ----------------------------------------------------------

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    double StartTime = 0.0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    double EndTime = 0.0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    FName Winner = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    EDragonWinCondition WinCondition = EDragonWinCondition::LethalDamage;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    bool bFinalized = false;

    // --- Aggregates -------------------------------------------------------

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    TMap<FName, FDragonFighterRoundStats> FighterStats;

    /** Applied damage summed per struck body part. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    TMap<EDragonBodyPart, float> DamageByPart;

    /** Count of attacks by ability tag. Feeds the pre/post-impairment split (F6). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    TMap<FGameplayTag, int32> AbilityUsage;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    int32 EventCount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    float TotalAppliedDamage = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Round")
    TArray<FDragonAssessmentTracePoint> AssessmentTrace;

    // --- Lifecycle --------------------------------------------------------

    void BeginRound(FName InEncounterId, int32 InRoundId, FName InArenaId,
                    const FDragonFighterProfile& InA, const FDragonFighterProfile& InB,
                    double InStartTime);

    /** Record a win condition and end time. WinCondition is a closed enum, so an
     *  out-of-range raw value is rejected and the round is left unfinalized. */
    void Finalize(FName InWinner, EDragonWinCondition InWinCondition, double InEndTime);

    // --- Ingestion (the single event path) --------------------------------

    /** Fold one combat event into the body-part damage distribution and ability
     *  usage counts. This is the only aggregation driven straight off the event
     *  stream; everything else is an explicit Record/Sample call. */
    void IngestEvent(const FDragonCombatEvent& Event);

    // --- Explicit sampling / counters -------------------------------------

    void SampleOccupancy(FName Fighter, EDragonLocomotionState Locomotion, FIntPoint Cell,
                         float DeltaSeconds, bool bInContact);

    void RecordStaminaExhaustion(FName Fighter);
    void RecordDisengage(FName Fighter);
    void RecordReengage(FName Fighter);
    void RecordDiveAttempt(FName Fighter);
    void RecordDiveConversion(FName Fighter);
    void RecordGrappleAttempt(FName Fighter, EDragonGrappleOutcome Outcome);
    void RecordAssessment(const FDragonAssessmentTracePoint& Point);

    // --- Derived / queries ------------------------------------------------

    double Duration() const { return FMath::Max(0.0, EndTime - StartTime); }

    /** Fraction of the round the fighter spent in contact range. 0 for a
     *  zero-length round. Used by F2. */
    float ContactFraction(FName Fighter) const;

    const FDragonFighterRoundStats* FindStats(FName Fighter) const { return FighterStats.Find(Fighter); }

    /** Serialize to a compact JSON object (one line per round in the log). */
    TSharedRef<FJsonObject> ToJson() const;

private:
    FDragonFighterRoundStats& StatsFor(FName Fighter);
};

/**
 * Streams round summaries to a newline-delimited JSON file. Open once per batch,
 * append per round, flush to guarantee the round survives a crash mid-run. This
 * is the "round summary writer" of Docs/TELEMETRY.md instrumentation step 1.
 */
class DRAGONCYCLE_API FDragonRoundSummaryWriter
{
public:
    FDragonRoundSummaryWriter() = default;
    ~FDragonRoundSummaryWriter();

    FDragonRoundSummaryWriter(const FDragonRoundSummaryWriter&) = delete;
    FDragonRoundSummaryWriter& operator=(const FDragonRoundSummaryWriter&) = delete;

    /** Open (or create) the log at AbsolutePath. Appends by default so a batch
     *  run can accumulate across encounters. Returns false if the handle could
     *  not be created. */
    bool Open(const FString& AbsolutePath, bool bAppend = true);

    bool IsOpen() const { return Archive != nullptr; }

    /** Append one summary as a JSON line. No-op returning false if not open. */
    bool Append(const FDragonRoundSummary& Summary);

    /** Force buffered bytes to disk. */
    void Flush();

    void Close();

    int32 GetAppendedCount() const { return AppendedCount; }
    const FString& GetPath() const { return Path; }

    /** Pure serialization, exposed for testing: one summary to one JSON line
     *  (no trailing newline). */
    static FString SummaryToJsonLine(const FDragonRoundSummary& Summary);

private:
    FArchive* Archive = nullptr;
    FString Path;
    int32 AppendedCount = 0;
};
