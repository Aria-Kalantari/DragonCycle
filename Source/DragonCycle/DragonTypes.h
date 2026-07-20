#pragma once

#include "CoreMinimal.h"
#include "DragonTypes.generated.h"

UENUM(BlueprintType)
enum class EDragonMorphotype : uint8
{
    /** Heavy six-limbed quadruped. Quadrupedal launch, secured-bite leverage. */
    Drake,
    /** Two legs, wings are the forelimbs. Interception predator. */
    Wyvern,
    /** Legless serpentine body, long neck, reduced wings. Range-control duelist. */
    Longneck
};

/** How a morphotype gets airborne. Gates the takeoff ability, not just its cost. */
UENUM(BlueprintType)
enum class EDragonLaunchMode : uint8
{
    Quadrupedal,
    Bipedal,
    CoilSpring,
    None
};

UENUM(BlueprintType)
enum class EDragonLifeStage : uint8
{
    Hatchling,
    Juvenile,
    PrimeAdult,
    Elder,
    Ancient
};

UENUM(BlueprintType)
enum class EDragonExperienceRank : uint8
{
    Naive,
    Seasoned,
    Veteran,
    Master,
    Legendary
};

UENUM(BlueprintType)
enum class EDragonBodyPart : uint8
{
    Head,
    Jaw,
    Neck,
    Torso,
    WingLeft,
    WingRight,
    ForelimbLeft,
    ForelimbRight,
    HindLimbLeft,
    HindLimbRight,
    Tail
};

UENUM(BlueprintType)
enum class EDragonDamageMode : uint8
{
    Blunt,
    Cut,
    Puncture,
    Heat,
    Chemical
};

USTRUCT(BlueprintType)
struct FDragonBodyPartProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body")
    EDragonBodyPart Part = EDragonBodyPart::Torso;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body", meta=(ClampMin="1.0"))
    float MaxIntegrity = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body", meta=(ClampMin="0.0", ClampMax="1.0"))
    float Armor = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body", meta=(ClampMin="0.0", ClampMax="1.0"))
    float ImpairmentThreshold = 0.4f;
};

/**
 * Limb topology from the schema's body_plan block. Counts are canonical per
 * morphotype and enforced by Tools/validate_data.py; they live here so the
 * imported asset, not the JSON, is what runtime systems read (ADR-006).
 */
USTRUCT(BlueprintType)
struct FDragonBodyPlan
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body", meta=(ClampMin="0", ClampMax="4"))
    int32 Legs = 4;

    /** Wings that are not limbs. Drake and longneck pattern. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body", meta=(ClampMin="0", ClampMax="2"))
    int32 IndependentWings = 2;

    /** Forelimbs that are the wings. Wyvern pattern; excludes separate forelimb parts. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body", meta=(ClampMin="0", ClampMax="2"))
    int32 WingArms = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body", meta=(ClampMin="1", ClampMax="1"))
    int32 Heads = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body")
    bool bSerpentine = false;
};

/** Starting resource pools. Consumed by the attribute set at spawn, not live state. */
USTRUCT(BlueprintType)
struct FDragonResourceBaseline
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resources", meta=(ClampMin="1.0"))
    float Health = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resources", meta=(ClampMin="1.0"))
    float Stamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resources", meta=(ClampMin="1.0"))
    float HeatCapacity = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resources", meta=(ClampMin="0.0"))
    float BreathReserve = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resources", meta=(ClampMin="1.0"))
    float Balance = 100.0f;
};

/**
 * Species instinct weights for utility scoring, 0-1. The assessment-related
 * weights (self bias, resource value, persistence, mutual-assessment cap,
 * skill) live in FDragonAssessment instead; these are tactical preferences.
 */
USTRUCT(BlueprintType)
struct FDragonAIPersonality
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI", meta=(ClampMin="0.0", ClampMax="1.0"))
    float GroundAggression = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI", meta=(ClampMin="0.0", ClampMax="1.0"))
    float AirAggression = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI", meta=(ClampMin="0.0", ClampMax="1.0"))
    float Grapple = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI", meta=(ClampMin="0.0", ClampMax="1.0"))
    float InjuryProtection = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI", meta=(ClampMin="0.0", ClampMax="1.0"))
    float DisplayPreference = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI", meta=(ClampMin="0.0", ClampMax="1.0"))
    float TerrainExploitation = 0.5f;
};

USTRUCT(BlueprintType)
struct FDragonBodyPartState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Body")
    float CurrentIntegrity = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Body")
    float MaxIntegrity = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Body")
    bool bImpaired = false;

    float GetIntegrityRatio() const
    {
        return MaxIntegrity > KINDA_SMALL_NUMBER
            ? FMath::Clamp(CurrentIntegrity / MaxIntegrity, 0.0f, 1.0f)
            : 0.0f;
    }
};

USTRUCT(BlueprintType)
struct FDragonLocalizedDamage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage", meta=(ClampMin="0.0"))
    float RawDamage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage", meta=(ClampMin="0.0", ClampMax="1.0"))
    float Penetration = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
    EDragonDamageMode Mode = EDragonDamageMode::Blunt;
};

USTRUCT(BlueprintType)
struct FDragonDamageResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Damage")
    float AppliedIntegrityDamage = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Damage")
    float RemainingIntegrityRatio = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Damage")
    bool bNewlyImpaired = false;
};

/**
 * Contest assessment state. Drives whether a fight is worth continuing, not
 * how hard the attacks land.
 *
 * Implements the cumulative assessment model: withdrawal is triggered by
 * accumulated cost measured against a threshold set by resource value and
 * persistence. Opponent-reading modifies that threshold but does not drive it,
 * because self and cumulative assessment are better supported across species
 * than full mutual assessment. See Docs/COMBAT_MODEL.md and sources 75, 78,
 * 79, 81 in Docs/RESEARCH_BIBLIOGRAPHY.md.
 *
 * Higher difficulty raises the quality of this judgement. It never grants
 * access to hidden player input, extra health, or faster reactions.
 */
USTRUCT(BlueprintType)
struct FDragonAssessment
{
    GENERATED_BODY()

    /** Primary driver. Stamina spent, integrity lost, position surrendered. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Assessment")
    float AccruedCost = 0.0f;

    /** Disengage above this. Derived from ResourceValue and Persistence. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Assessment")
    float CostThreshold = 1.0f;

    /** Estimate of own fighting ability, 0-1, from observable state only. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Assessment")
    float SelfCapability = 1.0f;

    /** Systematic error in SelfCapability. Naive individuals skew positive. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Assessment", meta=(ClampMin="-0.5", ClampMax="0.5"))
    float SelfBias = 0.0f;

    /** Estimate of the opponent's fighting ability, from observed behaviour. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Assessment")
    float OpponentCapability = 1.0f;

    /** How far OpponentCapability may move CostThreshold. Rises with experience. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Assessment", meta=(ClampMin="0.0", ClampMax="1.0"))
    float MutualAssessmentWeight = 0.3f;

    /** Subjective value of what is being contested. Raises CostThreshold. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Assessment", meta=(ClampMin="0.0", ClampMax="1.0"))
    float ResourceValue = 0.5f;

    /** Willingness to continue an unfavourable exchange. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Assessment", meta=(ClampMin="0.0", ClampMax="1.0"))
    float Persistence = 0.5f;

    /** Accuracy, tactic selection, decision speed. Distinct from raw capability. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Assessment", meta=(ClampMin="0.0", ClampMax="1.0"))
    float Skill = 0.5f;

    /**
     * Running tally of recent contest outcomes, decaying toward zero.
     * Winner and loser effects update self-assessment like a leaky integrator,
     * so a recent throw should lower SelfCapability and then recover.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Assessment")
    float OutcomeMemory = 0.0f;

    /** True when accumulated cost has passed the threshold. */
    bool ShouldDisengage() const
    {
        return AccruedCost > CostThreshold;
    }
};
