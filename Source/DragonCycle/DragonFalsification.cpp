#include "DragonFalsification.h"

#include <limits>

namespace DragonFalsification
{
    double HeatmapCorrelation(const TMap<FIntPoint, float>& A, const TMap<FIntPoint, float>& B)
    {
        // Union of occupied cells; a cell absent from one map contributes zero
        // there, which is the correct "no time spent" reading.
        TSet<FIntPoint> Cells;
        Cells.Reserve(A.Num() + B.Num());
        for (const TPair<FIntPoint, float>& Pair : A) { Cells.Add(Pair.Key); }
        for (const TPair<FIntPoint, float>& Pair : B) { Cells.Add(Pair.Key); }

        const int32 N = Cells.Num();
        if (N == 0)
        {
            return 0.0;
        }

        double SumA = 0.0, SumB = 0.0;
        for (const FIntPoint& Cell : Cells)
        {
            SumA += A.FindRef(Cell);
            SumB += B.FindRef(Cell);
        }
        const double MeanA = SumA / N;
        const double MeanB = SumB / N;

        double Cov = 0.0, VarA = 0.0, VarB = 0.0;
        for (const FIntPoint& Cell : Cells)
        {
            const double Da = A.FindRef(Cell) - MeanA;
            const double Db = B.FindRef(Cell) - MeanB;
            Cov += Da * Db;
            VarA += Da * Da;
            VarB += Db * Db;
        }

        const double Denom = FMath::Sqrt(VarA * VarB);
        if (Denom <= UE_DOUBLE_SMALL_NUMBER)
        {
            return 0.0;
        }
        return Cov / Denom;
    }

    double Median(TArray<double> Values)
    {
        if (Values.Num() == 0)
        {
            return 0.0;
        }
        Values.Sort();
        const int32 Mid = Values.Num() / 2;
        if (Values.Num() % 2 == 1)
        {
            return Values[Mid];
        }
        return 0.5 * (Values[Mid - 1] + Values[Mid]);
    }
}

namespace
{
    using namespace DragonFalsification;

    const double NaNMarker = std::numeric_limits<double>::quiet_NaN();

    const FDragonFighterProfile* ProfileById(const FDragonRoundSummary& Summary, FName Id)
    {
        if (Summary.FighterA.Id == Id) { return &Summary.FighterA; }
        if (Summary.FighterB.Id == Id) { return &Summary.FighterB; }
        return nullptr;
    }

    bool IsMatchup(const FDragonRoundSummary& Summary, EDragonMorphotype X, EDragonMorphotype Y)
    {
        const EDragonMorphotype A = Summary.FighterA.Morphotype;
        const EDragonMorphotype B = Summary.FighterB.Morphotype;
        return (A == X && B == Y) || (A == Y && B == X);
    }

    FDragonFalsificationResult MakeResult(EDragonFalsificationId Id, const TCHAR* Statement, double Threshold)
    {
        FDragonFalsificationResult R;
        R.Id = Id;
        R.Statement = Statement;
        R.Threshold = Threshold;
        R.Measured = NaNMarker;
        R.Verdict = EDragonFalsificationVerdict::Insufficient;
        return R;
    }

    // F1: drake lethal-damage wins as a fraction of drake wins.
    FDragonFalsificationResult BuildF1(TArrayView<const FDragonRoundSummary> Summaries)
    {
        FDragonFalsificationResult R = MakeResult(
            EDragonFalsificationId::F1,
            TEXT("The heaviest morphotype wins mostly through durability."),
            F1_DrakeLethalWinFraction);

        int32 DrakeWins = 0, DrakeLethalWins = 0;
        for (const FDragonRoundSummary& S : Summaries)
        {
            if (!S.bFinalized) { continue; }
            const FDragonFighterProfile* Winner = ProfileById(S, S.Winner);
            if (!Winner || Winner->Morphotype != EDragonMorphotype::Drake) { continue; }
            ++DrakeWins;
            if (S.WinCondition == EDragonWinCondition::LethalDamage) { ++DrakeLethalWins; }
        }

        if (DrakeWins == 0)
        {
            R.Detail = TEXT("No finalized drake wins yet.");
            return R;
        }
        R.Measured = static_cast<double>(DrakeLethalWins) / DrakeWins;
        R.Verdict = R.Measured > F1_DrakeLethalWinFraction
            ? EDragonFalsificationVerdict::Fail : EDragonFalsificationVerdict::Pass;
        R.Detail = FString::Printf(TEXT("%d/%d drake wins were lethal-damage."), DrakeLethalWins, DrakeWins);
        return R;
    }

    // F2: mean drake time-in-contact fraction in drake-vs-wyvern rounds.
    FDragonFalsificationResult BuildF2(TArrayView<const FDragonRoundSummary> Summaries)
    {
        FDragonFalsificationResult R = MakeResult(
            EDragonFalsificationId::F2,
            TEXT("The lightest morphotype wins mostly through kiting."),
            F2_DrakeContactFraction);

        double Sum = 0.0;
        int32 Count = 0;
        for (const FDragonRoundSummary& S : Summaries)
        {
            if (!S.bFinalized || S.Duration() <= 0.0) { continue; }
            if (!IsMatchup(S, EDragonMorphotype::Drake, EDragonMorphotype::Wyvern)) { continue; }

            const FName DrakeId = (S.FighterA.Morphotype == EDragonMorphotype::Drake)
                ? S.FighterA.Id : S.FighterB.Id;
            const FDragonFighterRoundStats* Stats = S.FindStats(DrakeId);
            // Require occupancy to have been sampled; otherwise a 0 here is
            // "unmeasured", not "never in contact".
            if (!Stats || Stats->TotalTracked() <= 0.0f) { continue; }

            Sum += S.ContactFraction(DrakeId);
            ++Count;
        }

        if (Count == 0)
        {
            R.Detail = TEXT("No sampled drake-vs-wyvern rounds yet.");
            return R;
        }
        R.Measured = Sum / Count;
        R.Verdict = R.Measured < F2_DrakeContactFraction
            ? EDragonFalsificationVerdict::Fail : EDragonFalsificationVerdict::Pass;
        R.Detail = FString::Printf(TEXT("Mean drake contact fraction over %d rounds."), Count);
        return R;
    }

    // F4: correlation of aggregate occupancy heatmaps across the two arenas.
    FDragonFalsificationResult BuildF4(TArrayView<const FDragonRoundSummary> Summaries)
    {
        FDragonFalsificationResult R = MakeResult(
            EDragonFalsificationId::F4,
            TEXT("Terrain does not change behaviour."),
            F4_HeatmapCorrelation);

        // Aggregate every fighter's occupancy per arena.
        TMap<FName, TMap<FIntPoint, float>> ByArena;
        for (const FDragonRoundSummary& S : Summaries)
        {
            TMap<FIntPoint, float>& Arena = ByArena.FindOrAdd(S.ArenaId);
            for (const TPair<FName, FDragonFighterRoundStats>& FS : S.FighterStats)
            {
                for (const TPair<FIntPoint, float>& Cell : FS.Value.TerrainOccupancy)
                {
                    Arena.FindOrAdd(Cell.Key) += Cell.Value;
                }
            }
        }

        if (ByArena.Num() < 2)
        {
            R.Detail = TEXT("Need occupancy from two arenas.");
            return R;
        }

        // Pick the two most-occupied arenas deterministically.
        TArray<TPair<FName, TMap<FIntPoint, float>>> Arenas;
        for (TPair<FName, TMap<FIntPoint, float>>& Pair : ByArena)
        {
            Arenas.Add(MoveTemp(Pair));
        }
        Arenas.Sort([](const TPair<FName, TMap<FIntPoint, float>>& L, const TPair<FName, TMap<FIntPoint, float>>& Rt)
        {
            return L.Key.LexicalLess(Rt.Key);
        });

        R.Measured = HeatmapCorrelation(Arenas[0].Value, Arenas[1].Value);
        R.Verdict = R.Measured > F4_HeatmapCorrelation
            ? EDragonFalsificationVerdict::Fail : EDragonFalsificationVerdict::Pass;
        R.Detail = FString::Printf(TEXT("Occupancy correlation between %s and %s."),
                                   *Arenas[0].Key.ToString(), *Arenas[1].Key.ToString());
        return R;
    }

    // F8: largest single win-condition share across all finalized rounds.
    FDragonFalsificationResult BuildF8(TArrayView<const FDragonRoundSummary> Summaries)
    {
        FDragonFalsificationResult R = MakeResult(
            EDragonFalsificationId::F8,
            TEXT("One win condition dominates."),
            F8_WinConditionShare);

        int32 Counts[5] = { 0, 0, 0, 0, 0 };
        int32 Total = 0;
        for (const FDragonRoundSummary& S : Summaries)
        {
            if (!S.bFinalized) { continue; }
            ++Counts[static_cast<uint8>(S.WinCondition)];
            ++Total;
        }

        if (Total == 0)
        {
            R.Detail = TEXT("No finalized rounds yet.");
            return R;
        }
        int32 MaxCount = 0;
        for (int32 C : Counts) { MaxCount = FMath::Max(MaxCount, C); }
        R.Measured = static_cast<double>(MaxCount) / Total;
        R.Verdict = R.Measured > F8_WinConditionShare
            ? EDragonFalsificationVerdict::Fail : EDragonFalsificationVerdict::Pass;
        R.Detail = FString::Printf(TEXT("Top win condition is %.0f%% of %d rounds."), R.Measured * 100.0, Total);
        return R;
    }

    // F9: ancient win rate in equal-experience rounds where one fighter is an
    //     ancient and the other is not.
    FDragonFalsificationResult BuildF9(TArrayView<const FDragonRoundSummary> Summaries)
    {
        FDragonFalsificationResult R = MakeResult(
            EDragonFalsificationId::F9,
            TEXT("Ancients simply beat prime adults."),
            F9_AncientWinRate);

        int32 Qualifying = 0, AncientWins = 0;
        for (const FDragonRoundSummary& S : Summaries)
        {
            if (!S.bFinalized) { continue; }
            if (S.FighterA.Experience != S.FighterB.Experience) { continue; }

            const bool bAIsAncient = S.FighterA.LifeStage == EDragonLifeStage::Ancient;
            const bool bBIsAncient = S.FighterB.LifeStage == EDragonLifeStage::Ancient;
            if (bAIsAncient == bBIsAncient) { continue; } // need exactly one ancient

            ++Qualifying;
            const FDragonFighterProfile* Winner = ProfileById(S, S.Winner);
            if (Winner && Winner->LifeStage == EDragonLifeStage::Ancient) { ++AncientWins; }
        }

        if (Qualifying == 0)
        {
            R.Detail = TEXT("No equal-experience ancient-vs-non-ancient rounds yet.");
            return R;
        }
        R.Measured = static_cast<double>(AncientWins) / Qualifying;
        R.Verdict = R.Measured > F9_AncientWinRate
            ? EDragonFalsificationVerdict::Fail : EDragonFalsificationVerdict::Pass;
        R.Detail = FString::Printf(TEXT("%d/%d qualifying rounds won by the ancient."), AncientWins, Qualifying);
        return R;
    }

    // F10: median round duration against the 5-10 minute band.
    FDragonFalsificationResult BuildF10(TArrayView<const FDragonRoundSummary> Summaries)
    {
        FDragonFalsificationResult R = MakeResult(
            EDragonFalsificationId::F10,
            TEXT("Realism cost the fun."),
            F10_MinSeconds);

        TArray<double> Durations;
        for (const FDragonRoundSummary& S : Summaries)
        {
            if (S.bFinalized) { Durations.Add(S.Duration()); }
        }

        if (Durations.Num() == 0)
        {
            R.Detail = TEXT("No finalized rounds yet. Engagement clause needs playtest data.");
            return R;
        }
        R.Measured = Median(Durations);
        const bool bOutsideBand = R.Measured < F10_MinSeconds || R.Measured > F10_MaxSeconds;
        R.Verdict = bOutsideBand ? EDragonFalsificationVerdict::Fail : EDragonFalsificationVerdict::Pass;
        R.Detail = FString::Printf(
            TEXT("Median round %.0fs; target %.0f-%.0fs. Engagement-drop clause is a separate playtest measure."),
            R.Measured, F10_MinSeconds, F10_MaxSeconds);
        return R;
    }

    // Metrics that require an external protocol or a data axis the slice has not
    // produced yet. Encoded with their exact thresholds; Insufficient until the
    // relevant harness exists (instrumentation steps 4-5 in TELEMETRY.md).
    FDragonFalsificationResult BuildExternal(EDragonFalsificationId Id, const TCHAR* Statement,
                                             double Threshold, const TCHAR* Detail)
    {
        FDragonFalsificationResult R = MakeResult(Id, Statement, Threshold);
        R.Detail = Detail;
        return R;
    }
}

FDragonFalsificationReport FDragonFalsificationReport::Build(TArrayView<const FDragonRoundSummary> Summaries)
{
    FDragonFalsificationReport Report;
    Report.Results.Add(BuildF1(Summaries));
    Report.Results.Add(BuildF2(Summaries));
    Report.Results.Add(BuildExternal(
        EDragonFalsificationId::F3,
        TEXT("Observers cannot name the morphotype from behaviour."),
        F3_ObserverCorrectFraction,
        TEXT("Requires the blind observer protocol (TELEMETRY.md step 5).")));
    Report.Results.Add(BuildF4(Summaries));
    Report.Results.Add(BuildExternal(
        EDragonFalsificationId::F5,
        TEXT("Difficulty raises stats rather than judgement."),
        0.0,
        TEXT("Requires a difficulty sweep with disengage-timing quality; no numeric threshold.")));
    Report.Results.Add(BuildExternal(
        EDragonFalsificationId::F6,
        TEXT("Body damage does not change tactics."),
        0.0,
        TEXT("Requires the pre/post-impairment ability split; no numeric threshold.")));
    Report.Results.Add(BuildExternal(
        EDragonFalsificationId::F7,
        TEXT("Stamina only matters at zero."),
        F7_StaminaBandLow,
        TEXT("Requires behaviour sampling in the 30-60% stamina band.")));
    Report.Results.Add(BuildF8(Summaries));
    Report.Results.Add(BuildF9(Summaries));
    Report.Results.Add(BuildF10(Summaries));
    return Report;
}

const FDragonFalsificationResult* FDragonFalsificationReport::Find(EDragonFalsificationId Id) const
{
    return Results.FindByPredicate([Id](const FDragonFalsificationResult& R) { return R.Id == Id; });
}
