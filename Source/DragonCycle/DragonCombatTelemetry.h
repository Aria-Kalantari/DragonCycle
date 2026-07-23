#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DragonTypes.h"
#include "DragonCombatTelemetry.generated.h"

/**
 * Telemetry core types (ADR-010, Docs/TELEMETRY.md).
 *
 * This is the measuring instrument, not the thing it measures. Nothing here
 * computes an attack, an ability, or an AI decision; it only records what those
 * systems will later report. One combat event feeds animation, VFX, audio, UI,
 * AI, and telemetry alike (see UDragonTelemetrySubsystem). Adding a second event
 * source is forbidden because it will drift.
 */

/**
 * How committed an action is. Drives recovery length and punish window. This is
 * the same closed set the authoring schema uses (Data/Schemas/attack.schema.json
 * commit_class enum); the two must be changed together.
 */
UENUM(BlueprintType)
enum class EDragonCommitClass : uint8
{
    /** Fast, low commitment. Contests space without surrendering stance. */
    Probe,
    /** Ordinary attack. Moderate recovery. */
    Standard,
    /** Heavy commitment. Readable before the window opens, long recovery. */
    Committed
};

/**
 * The five ways a round can end (Docs/TELEMETRY.md). Exactly these values and no
 * others: a slice where one condition dominates has a balance problem the
 * aggregate numbers would otherwise hide (threshold F8).
 */
UENUM(BlueprintType)
enum class EDragonWinCondition : uint8
{
    /** Loser's flight was disabled. */
    WingDisable,
    /** Loser ran out of stamina and could no longer fight. */
    StaminaAttrition,
    /** Loser reached lethal health loss. */
    LethalDamage,
    /** Loser was killed by terrain (cliff, fall, hazard). */
    TerrainKill,
    /** Loser broke off and fled; winner held the field. */
    OpponentDisengage
};

/** Coarse locomotion bucket for time-in-mode accounting. Derived from the
 *  State.Grounded / State.Airborne / State.Grappled gameplay tags at the sampling
 *  boundary so the summary math itself stays tag-free and unit-testable. */
UENUM(BlueprintType)
enum class EDragonLocomotionState : uint8
{
    Grounded,
    Airborne,
    Grappled
};

/** Outcome buckets for a grapple exchange (SPEC M4.5: pin/throw, disengage,
 *  cliff or aerial fall, plus a clean break). Reporting categories only — the
 *  grapple rules live in Milestone 4, not here. */
UENUM(BlueprintType)
enum class EDragonGrappleOutcome : uint8
{
    Pin,
    Throw,
    Escape,
    Fall,
    Broken
};

/**
 * Per-damage-mode breakdown of a single contact ("damage layers" in the schema).
 * The five layers are the EDragonDamageMode set; this struct is deliberately a
 * parallel of that enum so a caller cannot report a layer that has no mode.
 */
USTRUCT(BlueprintType)
struct FDragonDamageLayers
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Damage", meta=(ClampMin="0.0"))
    float Blunt = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Damage", meta=(ClampMin="0.0"))
    float Cut = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Damage", meta=(ClampMin="0.0"))
    float Puncture = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Damage", meta=(ClampMin="0.0"))
    float Heat = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Damage", meta=(ClampMin="0.0"))
    float Chemical = 0.0f;

    /** Sum across all layers. Not the same as applied damage, which is post-armor. */
    float Total() const { return Blunt + Cut + Puncture + Heat + Chemical; }

    float ForMode(EDragonDamageMode Mode) const
    {
        switch (Mode)
        {
        case EDragonDamageMode::Blunt:    return Blunt;
        case EDragonDamageMode::Cut:      return Cut;
        case EDragonDamageMode::Puncture: return Puncture;
        case EDragonDamageMode::Heat:     return Heat;
        case EDragonDamageMode::Chemical: return Chemical;
        default:                          return 0.0f;
        }
    }
};

/**
 * The single combat event (Docs/TELEMETRY.md "Event schema"). Constructed once
 * per meaningful contact and routed through UDragonTelemetrySubsystem, which is
 * the only origin for animation, VFX, audio, UI, AI, and telemetry.
 *
 * Every field on the schema is present. target_body_part uses EDragonBodyPart —
 * the same closed 11-part set the M0.6 importer enforces — and AbilityTag / the
 * state tags use the gameplay tags already declared in Config/DefaultEngine.ini.
 */
USTRUCT(BlueprintType)
struct FDragonCombatEvent
{
    GENERATED_BODY()

    // --- Identity ---------------------------------------------------------

    /** Seconds since world start at the moment of contact. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    double Timestamp = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FName EncounterId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    int32 RoundId = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FName AttackerId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FName TargetId = NAME_None;

    /** Morphotype pair, attacker first. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    EDragonMorphotype AttackerMorphotype = EDragonMorphotype::Drake;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    EDragonMorphotype TargetMorphotype = EDragonMorphotype::Drake;

    // --- Action -----------------------------------------------------------

    /** SPEC 13.3 Ability.Attack.* / Ability.Movement.* tag. Verified, not redeclared. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FGameplayTag AbilityTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    EDragonCommitClass CommitClass = EDragonCommitClass::Standard;

    // --- Contact ----------------------------------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    EDragonBodyPart TargetBodyPart = EDragonBodyPart::Torso;

    /** Per-mode damage before armor/tissue response. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FDragonDamageLayers DamageLayers;

    /** Health/integrity damage actually applied after armor. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event", meta=(ClampMin="0.0"))
    float AppliedDamage = 0.0f;

    /** Balance (stagger) damage applied. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event", meta=(ClampMin="0.0"))
    float BalanceDamage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FVector ContactPoint = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FVector ContactNormal = FVector::ZeroVector;

    /** Attacker velocity relative to target at contact. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FVector RelativeVelocity = FVector::ZeroVector;

    // --- Attacker resource state at contact -------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event", meta=(ClampMin="0.0"))
    float AttackerStamina = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event", meta=(ClampMin="0.0"))
    float AttackerHeat = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event", meta=(ClampMin="0.0"))
    float AttackerBreath = 0.0f;

    // --- Consequence ------------------------------------------------------

    /** Impairments produced by this hit, expressed as Damage.* gameplay tags
     *  (SPEC 13.3). Reuses the declared damage vocabulary; no parallel list. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FGameplayTagContainer ImpairmentFlags;

    /** State.* tags active on attacker/target at contact (SPEC 13.3). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FGameplayTagContainer AttackerStateTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FGameplayTagContainer TargetStateTags;

    // --- Spatial context --------------------------------------------------

    /** Integer terrain-grid cell of the contact (occupancy heatmap key). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    FIntPoint TerrainCell = FIntPoint::ZeroValue;

    /** Attacker altitude above ground level, metres. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Event")
    float AltitudeAGL = 0.0f;
};

/**
 * One point on the assessment trace (Docs/TELEMETRY.md RoundSummary). Records
 * the capability estimates and the disengage decision that produced them, so a
 * withdrawal can be explained after the fact. The assessment model itself is
 * Milestone 5; this only stores what it emits.
 */
USTRUCT(BlueprintType)
struct FDragonAssessmentTracePoint
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Assessment")
    double Timestamp = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Assessment")
    FName Fighter = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Assessment")
    float SelfCapability = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Assessment")
    float OpponentCapability = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Assessment")
    float AccruedCost = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Assessment")
    float CostThreshold = 1.0f;

    /** True on the tick the fighter decided to break off. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Assessment")
    bool bDisengageDecision = false;
};

/** Static metadata for one fighter in a round. Enough to group summaries by
 *  morphotype, life stage, experience, and difficulty for the falsification
 *  report (F1, F5, F9) without re-deriving it from the event stream. */
USTRUCT(BlueprintType)
struct FDragonFighterProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Fighter")
    FName Id = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Fighter")
    EDragonMorphotype Morphotype = EDragonMorphotype::Drake;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Fighter")
    EDragonLifeStage LifeStage = EDragonLifeStage::PrimeAdult;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Fighter")
    EDragonExperienceRank Experience = EDragonExperienceRank::Seasoned;

    /** Judgement tier this fighter ran at (ADR-009). Difficulty scales judgement
     *  only; it is recorded here so F5 can check it never scaled the outcome. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Telemetry|Fighter")
    FName DifficultyTier = NAME_None;
};

namespace DragonTelemetry
{
    /** Resolve the SPEC 13.3 State.* tags into a locomotion bucket. Requests the
     *  ini-declared tags rather than redeclaring them; returns Grounded when no
     *  locomotion state tag is present. */
    DRAGONCYCLE_API EDragonLocomotionState LocomotionFromStateTags(const FGameplayTagContainer& StateTags);

    /** Machine-stable identifier for a win condition (used in the JSON log). */
    DRAGONCYCLE_API const TCHAR* WinConditionToString(EDragonWinCondition Condition);

    /** True for every value of the closed EDragonWinCondition set. The five
     *  values in Docs/TELEMETRY.md are exactly the enumerators, so a value is
     *  valid iff it is in range — this guards against a raw cast from disk. */
    DRAGONCYCLE_API bool IsValidWinCondition(uint8 RawValue);
}
