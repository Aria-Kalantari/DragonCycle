#pragma once

#include "CoreMinimal.h"
#include "DragonRoundSummary.h"
#include "DragonFalsification.generated.h"

/**
 * The ten falsification thresholds F1-F10 (Docs/TELEMETRY.md), encoded as
 * reportable metrics. They land now, with the instrument, even though nothing
 * emits events yet — so the numbers can never be quietly softened, rounded, or
 * re-derived to make a later balance pass easier. The thresholds below are the
 * document's numbers verbatim; a Python contract test asserts they still match.
 *
 * These are written as FAILURE conditions on purpose: it is easier to be honest
 * about a threshold you can breach than a goal you can approach.
 */
namespace DragonFalsification
{
    // F1: the heaviest morphotype wins mostly through durability.
    //     Fails if drake lethal-damage wins exceed this fraction.
    constexpr double F1_DrakeLethalWinFraction = 0.60;

    // F2: the lightest morphotype wins mostly through kiting.
    //     Fails if drake time-in-contact drops below this fraction of the round
    //     in drake-vs-wyvern.
    constexpr double F2_DrakeContactFraction = 0.25;

    // F3: observers cannot name the morphotype from behaviour.
    //     Fails below this fraction correct within three rounds (blind test).
    constexpr double F3_ObserverCorrectFraction = 0.80;

    // F4: terrain does not change behaviour.
    //     Fails if occupancy heatmaps correlate above this across the two maps.
    constexpr double F4_HeatmapCorrelation = 0.85;

    // F7: stamina only matters at zero.
    //     Fails if there is no measurable behaviour change in this stamina band.
    constexpr double F7_StaminaBandLow = 0.30;
    constexpr double F7_StaminaBandHigh = 0.60;

    // F8: one win condition dominates.
    //     Fails if any single win_condition exceeds this fraction across all matchups.
    constexpr double F8_WinConditionShare = 0.55;

    // F9: ancients simply beat prime adults.
    //     Fails if ancient win rate exceeds this at equal experience.
    constexpr double F9_AncientWinRate = 0.60;

    // F10: realism cost the fun.
    //     Fails if the median round falls outside this 5-10 minute band (seconds).
    constexpr double F10_MinSeconds = 300.0;
    constexpr double F10_MaxSeconds = 600.0;

    /** Pearson correlation of two terrain heatmaps over the union of their cells
     *  (missing cells count as zero occupancy). Deterministic; used by F4 and
     *  covered directly by an automation test. Returns 0 when either series has
     *  no variance. */
    DRAGONCYCLE_API double HeatmapCorrelation(const TMap<FIntPoint, float>& A, const TMap<FIntPoint, float>& B);

    /** Median of a value set. Empty set returns 0. */
    DRAGONCYCLE_API double Median(TArray<double> Values);
}

UENUM(BlueprintType)
enum class EDragonFalsificationId : uint8
{
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10
};

UENUM(BlueprintType)
enum class EDragonFalsificationVerdict : uint8
{
    /** The failure condition was not met. */
    Pass,
    /** The failure condition was met: the system has failed this threshold. */
    Fail,
    /** Not enough data yet to evaluate (e.g. no events emitted, or the metric
     *  needs an external protocol such as the blind observer test). */
    Insufficient
};

USTRUCT(BlueprintType)
struct DRAGONCYCLE_API FDragonFalsificationResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Falsification")
    EDragonFalsificationId Id = EDragonFalsificationId::F1;

    /** The documented failure statement, verbatim. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Falsification")
    FString Statement;

    /** The threshold that separates pass from fail (or the low end of a band). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Falsification")
    double Threshold = 0.0;

    /** The measured value, when one could be computed. NaN when Insufficient. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Falsification")
    double Measured = 0.0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Falsification")
    EDragonFalsificationVerdict Verdict = EDragonFalsificationVerdict::Insufficient;

    /** How it was measured, or why it could not be. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Falsification")
    FString Detail;
};

/**
 * The full F1-F10 report, computed from a set of round summaries. Metrics that
 * the round summaries already contain (F1, F2, F4, F8, F9, F10) are computed;
 * metrics that require an external protocol or a data axis the slice has not
 * produced yet (F3 blind observer, F5 difficulty sweep, F6 pre/post-impairment
 * split, F7 stamina-band behaviour) are reported Insufficient with their exact
 * threshold recorded, so they are visible and cannot be forgotten.
 */
USTRUCT(BlueprintType)
struct DRAGONCYCLE_API FDragonFalsificationReport
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Telemetry|Falsification")
    TArray<FDragonFalsificationResult> Results;

    /** Build the report from finalized round summaries. Safe on an empty set:
     *  every threshold comes back Insufficient with its number intact. */
    static FDragonFalsificationReport Build(TArrayView<const FDragonRoundSummary> Summaries);

    const FDragonFalsificationResult* Find(EDragonFalsificationId Id) const;
};
