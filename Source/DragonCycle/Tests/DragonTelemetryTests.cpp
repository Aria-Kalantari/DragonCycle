#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "DragonCombatTelemetry.h"
#include "DragonRoundSummary.h"
#include "DragonFalsification.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

// These cover the deterministic aggregation math CLAUDE.md asks to be tested:
// durations, distributions, occupancy accumulation, correlation, median, the
// falsification computations, and win-condition validity. No engine tick, no
// world, no tags — every input is hand-authored so the arithmetic is pinned.

namespace
{
    // `auto` rather than a fixed integer type: UE 5.5+ makes EAutomationTestFlags
    // an enum class, so the combined value is not implicitly a uint32.
    constexpr auto TelemetryTestFlags =
        EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;

    FDragonFighterProfile MakeProfile(const TCHAR* Id, EDragonMorphotype Morphotype,
                                      EDragonLifeStage Stage = EDragonLifeStage::PrimeAdult,
                                      EDragonExperienceRank Exp = EDragonExperienceRank::Seasoned)
    {
        FDragonFighterProfile P;
        P.Id = FName(Id);
        P.Morphotype = Morphotype;
        P.LifeStage = Stage;
        P.Experience = Exp;
        return P;
    }

    FDragonRoundSummary MakeRound(const TCHAR* Encounter, int32 RoundId, const TCHAR* Arena,
                                  const FDragonFighterProfile& A, const FDragonFighterProfile& B,
                                  double Start, double End)
    {
        FDragonRoundSummary S;
        S.BeginRound(FName(Encounter), RoundId, FName(Arena), A, B, Start);
        S.EndTime = End; // duration without forcing a win condition
        return S;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDragonTelemetryDurationTest,
    "DragonCycle.Telemetry.Duration", TelemetryTestFlags)
bool FDragonTelemetryDurationTest::RunTest(const FString&)
{
    const FDragonFighterProfile A = MakeProfile(TEXT("A"), EDragonMorphotype::Drake);
    const FDragonFighterProfile B = MakeProfile(TEXT("B"), EDragonMorphotype::Wyvern);

    FDragonRoundSummary S = MakeRound(TEXT("enc"), 1, TEXT("caldera"), A, B, 10.0, 130.0);
    S.Finalize(A.Id, EDragonWinCondition::LethalDamage, 130.0);

    TestEqual(TEXT("duration is end minus start"), S.Duration(), 120.0);
    TestTrue(TEXT("round is finalized"), S.bFinalized);

    // A negative interval can never yield a negative duration.
    FDragonRoundSummary Bad = MakeRound(TEXT("enc"), 2, TEXT("caldera"), A, B, 50.0, 10.0);
    TestEqual(TEXT("duration clamps at zero"), Bad.Duration(), 0.0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDragonTelemetryWinConditionValidityTest,
    "DragonCycle.Telemetry.WinConditionValidity", TelemetryTestFlags)
bool FDragonTelemetryWinConditionValidityTest::RunTest(const FString&)
{
    // Exactly five values (Docs/TELEMETRY.md), indices 0..4 valid, 5+ invalid.
    for (uint8 V = 0; V <= 4; ++V)
    {
        TestTrue(FString::Printf(TEXT("value %d valid"), V), DragonTelemetry::IsValidWinCondition(V));
    }
    TestFalse(TEXT("value 5 invalid"), DragonTelemetry::IsValidWinCondition(5));
    TestFalse(TEXT("value 99 invalid"), DragonTelemetry::IsValidWinCondition(99));

    TestEqual(TEXT("opponent_disengage is the last enumerator"),
              static_cast<int32>(EDragonWinCondition::OpponentDisengage), 4);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDragonTelemetryDamageDistributionTest,
    "DragonCycle.Telemetry.DamageDistribution", TelemetryTestFlags)
bool FDragonTelemetryDamageDistributionTest::RunTest(const FString&)
{
    const FDragonFighterProfile A = MakeProfile(TEXT("A"), EDragonMorphotype::Drake);
    const FDragonFighterProfile B = MakeProfile(TEXT("B"), EDragonMorphotype::Wyvern);
    FDragonRoundSummary S = MakeRound(TEXT("enc"), 1, TEXT("caldera"), A, B, 0.0, 60.0);

    FDragonCombatEvent E;
    E.TargetBodyPart = EDragonBodyPart::WingLeft;
    E.AppliedDamage = 30.0f;
    S.IngestEvent(E);

    E.AppliedDamage = 20.0f;
    S.IngestEvent(E); // same part accumulates

    E.TargetBodyPart = EDragonBodyPart::Jaw;
    E.AppliedDamage = 15.0f;
    S.IngestEvent(E);

    TestEqual(TEXT("event count"), S.EventCount, 3);
    TestEqual(TEXT("wing-left damage sums"), S.DamageByPart.FindRef(EDragonBodyPart::WingLeft), 50.0f);
    TestEqual(TEXT("jaw damage"), S.DamageByPart.FindRef(EDragonBodyPart::Jaw), 15.0f);
    TestEqual(TEXT("total applied damage"), S.TotalAppliedDamage, 65.0f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDragonTelemetryOccupancyTest,
    "DragonCycle.Telemetry.Occupancy", TelemetryTestFlags)
bool FDragonTelemetryOccupancyTest::RunTest(const FString&)
{
    const FDragonFighterProfile A = MakeProfile(TEXT("A"), EDragonMorphotype::Drake);
    const FDragonFighterProfile B = MakeProfile(TEXT("B"), EDragonMorphotype::Wyvern);
    FDragonRoundSummary S = MakeRound(TEXT("enc"), 1, TEXT("caldera"), A, B, 0.0, 100.0);

    // Fighter A: 20s grounded in contact at (0,0), 10s airborne out of contact at (1,0),
    // then 30s more grounded in contact at (0,0).
    S.SampleOccupancy(A.Id, EDragonLocomotionState::Grounded, FIntPoint(0, 0), 20.0f, /*bInContact=*/true);
    S.SampleOccupancy(A.Id, EDragonLocomotionState::Airborne, FIntPoint(1, 0), 10.0f, /*bInContact=*/false);
    S.SampleOccupancy(A.Id, EDragonLocomotionState::Grounded, FIntPoint(0, 0), 30.0f, /*bInContact=*/true);
    // A zero/negative delta must be ignored.
    S.SampleOccupancy(A.Id, EDragonLocomotionState::Grappled, FIntPoint(0, 0), 0.0f, true);

    const FDragonFighterRoundStats* Stats = S.FindStats(A.Id);
    TestNotNull(TEXT("A has stats"), Stats);
    if (!Stats) { return false; }

    TestEqual(TEXT("time grounded accumulates"), Stats->TimeGrounded, 50.0f);
    TestEqual(TEXT("time airborne"), Stats->TimeAirborne, 10.0f);
    TestEqual(TEXT("time grappled unchanged"), Stats->TimeGrappled, 0.0f);
    TestEqual(TEXT("time in contact"), Stats->TimeInContact, 50.0f);
    TestEqual(TEXT("heatmap cell (0,0) accumulates"), Stats->TerrainOccupancy.FindRef(FIntPoint(0, 0)), 50.0f);
    TestEqual(TEXT("heatmap cell (1,0)"), Stats->TerrainOccupancy.FindRef(FIntPoint(1, 0)), 10.0f);

    // Contact fraction is time-in-contact over round duration (100s).
    TestEqual(TEXT("contact fraction"), S.ContactFraction(A.Id), 0.5f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDragonTelemetryDiveAndGrappleTest,
    "DragonCycle.Telemetry.DiveAndGrapple", TelemetryTestFlags)
bool FDragonTelemetryDiveAndGrappleTest::RunTest(const FString&)
{
    const FDragonFighterProfile A = MakeProfile(TEXT("A"), EDragonMorphotype::Wyvern);
    const FDragonFighterProfile B = MakeProfile(TEXT("B"), EDragonMorphotype::Drake);
    FDragonRoundSummary S = MakeRound(TEXT("enc"), 1, TEXT("caldera"), A, B, 0.0, 60.0);

    S.RecordDiveAttempt(A.Id);
    S.RecordDiveAttempt(A.Id);
    S.RecordDiveAttempt(A.Id);
    S.RecordDiveAttempt(A.Id);
    S.RecordDiveConversion(A.Id); // 1 of 4 converts

    const FDragonFighterRoundStats* AStats = S.FindStats(A.Id);
    TestNotNull(TEXT("A stats"), AStats);
    if (!AStats) { return false; }
    TestEqual(TEXT("dive conversion rate 1/4"), AStats->DiveConversionRate(), 0.25f);

    // Recording a conversion with no prior attempt must not push the rate above 1.
    S.RecordDiveConversion(B.Id);
    const FDragonFighterRoundStats* BStats = S.FindStats(B.Id);
    TestNotNull(TEXT("B stats"), BStats);
    if (!BStats) { return false; }
    TestEqual(TEXT("orphan conversion bumps attempt count"), BStats->DiveAttempts, 1);
    TestEqual(TEXT("conversion rate capped at 1"), BStats->DiveConversionRate(), 1.0f);

    S.RecordGrappleAttempt(B.Id, EDragonGrappleOutcome::Pin);
    S.RecordGrappleAttempt(B.Id, EDragonGrappleOutcome::Pin);
    S.RecordGrappleAttempt(B.Id, EDragonGrappleOutcome::Throw);
    TestEqual(TEXT("grapple attempts"), BStats->GrappleAttempts, 3);
    TestEqual(TEXT("pin outcomes"), BStats->GrappleOutcomes.FindRef(EDragonGrappleOutcome::Pin), 2);
    TestEqual(TEXT("throw outcomes"), BStats->GrappleOutcomes.FindRef(EDragonGrappleOutcome::Throw), 1);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDragonTelemetryCorrelationTest,
    "DragonCycle.Telemetry.HeatmapCorrelation", TelemetryTestFlags)
bool FDragonTelemetryCorrelationTest::RunTest(const FString&)
{
    TMap<FIntPoint, float> MapA;
    MapA.Add(FIntPoint(0, 0), 1.0f);
    MapA.Add(FIntPoint(1, 0), 2.0f);
    MapA.Add(FIntPoint(2, 0), 3.0f);

    // Identical shape -> perfect correlation.
    TestTrue(TEXT("identical maps correlate at 1"),
             FMath::IsNearlyEqual(DragonFalsification::HeatmapCorrelation(MapA, MapA), 1.0, 1e-9));

    // Perfectly inverted -> -1.
    TMap<FIntPoint, float> MapInv;
    MapInv.Add(FIntPoint(0, 0), 3.0f);
    MapInv.Add(FIntPoint(1, 0), 2.0f);
    MapInv.Add(FIntPoint(2, 0), 1.0f);
    TestTrue(TEXT("inverted maps correlate at -1"),
             FMath::IsNearlyEqual(DragonFalsification::HeatmapCorrelation(MapA, MapInv), -1.0, 1e-9));

    // No variance -> 0 by definition here.
    TMap<FIntPoint, float> Flat;
    Flat.Add(FIntPoint(0, 0), 5.0f);
    Flat.Add(FIntPoint(1, 0), 5.0f);
    TestEqual(TEXT("zero-variance correlation is 0"),
              DragonFalsification::HeatmapCorrelation(Flat, Flat), 0.0);

    // Empty inputs -> 0, never a divide-by-zero.
    TestEqual(TEXT("empty correlation is 0"),
              DragonFalsification::HeatmapCorrelation(TMap<FIntPoint, float>(), TMap<FIntPoint, float>()), 0.0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDragonTelemetryMedianTest,
    "DragonCycle.Telemetry.Median", TelemetryTestFlags)
bool FDragonTelemetryMedianTest::RunTest(const FString&)
{
    TestEqual(TEXT("empty median is 0"), DragonFalsification::Median({}), 0.0);
    TestEqual(TEXT("odd median"), DragonFalsification::Median({ 3.0, 1.0, 2.0 }), 2.0);
    TestEqual(TEXT("even median averages the middle two"),
              DragonFalsification::Median({ 10.0, 20.0, 30.0, 40.0 }), 25.0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDragonTelemetryFalsificationTest,
    "DragonCycle.Telemetry.Falsification", TelemetryTestFlags)
bool FDragonTelemetryFalsificationTest::RunTest(const FString&)
{
    const FDragonFighterProfile Drake = MakeProfile(TEXT("drake"), EDragonMorphotype::Drake);
    const FDragonFighterProfile Wyvern = MakeProfile(TEXT("wyvern"), EDragonMorphotype::Wyvern);

    // Empty input: every threshold is Insufficient, and the numbers survive intact.
    {
        FDragonFalsificationReport Report = FDragonFalsificationReport::Build(TArrayView<const FDragonRoundSummary>());
        TestEqual(TEXT("ten thresholds reported"), Report.Results.Num(), 10);
        const FDragonFalsificationResult* F1 = Report.Find(EDragonFalsificationId::F1);
        TestNotNull(TEXT("F1 present"), F1);
        if (F1)
        {
            TestEqual(TEXT("F1 threshold unchanged"), F1->Threshold, DragonFalsification::F1_DrakeLethalWinFraction);
            TestTrue(TEXT("F1 insufficient on empty"), F1->Verdict == EDragonFalsificationVerdict::Insufficient);
        }
    }

    // F8: five rounds all ending in lethal damage -> one condition is 100% -> Fail.
    {
        TArray<FDragonRoundSummary> Rounds;
        for (int32 i = 0; i < 5; ++i)
        {
            FDragonRoundSummary S = MakeRound(TEXT("enc"), i, TEXT("caldera"), Drake, Wyvern, 0.0, 400.0);
            S.Finalize(Drake.Id, EDragonWinCondition::LethalDamage, 400.0);
            Rounds.Add(S);
        }
        FDragonFalsificationReport Report = FDragonFalsificationReport::Build(Rounds);

        const FDragonFalsificationResult* F8 = Report.Find(EDragonFalsificationId::F8);
        TestNotNull(TEXT("F8 present"), F8);
        if (F8)
        {
            TestEqual(TEXT("F8 measured share 1.0"), F8->Measured, 1.0);
            TestTrue(TEXT("F8 fails when one condition dominates"), F8->Verdict == EDragonFalsificationVerdict::Fail);
        }

        // F1: all five drake wins were lethal -> 1.0 > 0.60 -> Fail.
        const FDragonFalsificationResult* F1 = Report.Find(EDragonFalsificationId::F1);
        if (F1)
        {
            TestEqual(TEXT("F1 measured 1.0"), F1->Measured, 1.0);
            TestTrue(TEXT("F1 fails"), F1->Verdict == EDragonFalsificationVerdict::Fail);
        }

        // F10: median duration 400s is inside the 300-600s band -> Pass.
        const FDragonFalsificationResult* F10 = Report.Find(EDragonFalsificationId::F10);
        if (F10)
        {
            TestEqual(TEXT("F10 median 400s"), F10->Measured, 400.0);
            TestTrue(TEXT("F10 passes inside the band"), F10->Verdict == EDragonFalsificationVerdict::Pass);
        }
    }

    // F10 fail: a round far outside the 5-10 minute band.
    {
        TArray<FDragonRoundSummary> Rounds;
        FDragonRoundSummary S = MakeRound(TEXT("enc"), 0, TEXT("caldera"), Drake, Wyvern, 0.0, 30.0);
        S.Finalize(Drake.Id, EDragonWinCondition::TerrainKill, 30.0);
        Rounds.Add(S);
        FDragonFalsificationReport Report = FDragonFalsificationReport::Build(Rounds);
        const FDragonFalsificationResult* F10 = Report.Find(EDragonFalsificationId::F10);
        if (F10)
        {
            TestTrue(TEXT("F10 fails below band"), F10->Verdict == EDragonFalsificationVerdict::Fail);
        }
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDragonTelemetryJsonRoundTripTest,
    "DragonCycle.Telemetry.JsonRoundTrip", TelemetryTestFlags)
bool FDragonTelemetryJsonRoundTripTest::RunTest(const FString&)
{
    const FDragonFighterProfile A = MakeProfile(TEXT("drake_1"), EDragonMorphotype::Drake);
    const FDragonFighterProfile B = MakeProfile(TEXT("wyvern_1"), EDragonMorphotype::Wyvern);
    FDragonRoundSummary S = MakeRound(TEXT("enc_7"), 3, TEXT("caldera"), A, B, 0.0, 480.0);

    FDragonCombatEvent E;
    E.TargetBodyPart = EDragonBodyPart::Neck;
    E.AppliedDamage = 42.0f;
    S.IngestEvent(E);
    S.SampleOccupancy(A.Id, EDragonLocomotionState::Grounded, FIntPoint(2, 3), 12.0f, true);
    S.Finalize(A.Id, EDragonWinCondition::StaminaAttrition, 480.0);

    const FString Line = FDragonRoundSummaryWriter::SummaryToJsonLine(S);
    TestTrue(TEXT("line is non-empty"), Line.Len() > 0);
    TestFalse(TEXT("line has no embedded newline"), Line.Contains(TEXT("\n")));

    TSharedPtr<FJsonObject> Parsed;
    TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(Line);
    TestTrue(TEXT("line parses back as JSON"), FJsonSerializer::Deserialize(Reader, Parsed) && Parsed.IsValid());
    if (!Parsed.IsValid()) { return false; }

    TestEqual(TEXT("encounter round-trips"), Parsed->GetStringField(TEXT("encounter_id")), FString(TEXT("enc_7")));
    TestEqual(TEXT("duration round-trips"), Parsed->GetNumberField(TEXT("duration")), 480.0);
    TestEqual(TEXT("win condition serialized by name"),
              Parsed->GetStringField(TEXT("win_condition")), FString(TEXT("stamina_attrition")));
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
