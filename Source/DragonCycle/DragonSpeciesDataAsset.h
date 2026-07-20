#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DragonTypes.h"
#include "DragonSpeciesDataAsset.generated.h"

UCLASS(BlueprintType)
class DRAGONCYCLE_API UDragonSpeciesDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
    FName SpeciesId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
    EDragonMorphotype Morphotype = EDragonMorphotype::Drake;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
    EDragonLifeStage LifeStage = EDragonLifeStage::PrimeAdult;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity")
    EDragonExperienceRank Experience = EDragonExperienceRank::Seasoned;

    /** Third aging axis (SPEC 7.3). 1 = uninjured and well-fed; degrades independently of life stage. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity", meta=(ClampMin="0.0", ClampMax="1.0"))
    float Condition = 1.0f;

    /** One-sentence fighting identity from the authoring JSON. Designer-facing, not UI text. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Identity", meta=(MultiLine="true"))
    FString CombatIdentity;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body")
    TArray<FDragonBodyPartProfile> BodyParts;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body")
    FDragonBodyPlan BodyPlan;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="1.0"))
    float MassKg = 6000.0f;

    /** Total lifting area. MassKg / WingAreaM2 is wing loading, which sets the speed/agility trade. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0"))
    float WingAreaM2 = 110.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
    EDragonLaunchMode LaunchMode = EDragonLaunchMode::Quadrupedal;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0"))
    float MaxTurnRateDegPerSecond = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0"))
    float StallSpeedMetersPerSecond = 22.0f;

    /** Neck segment count. Drives reach, off-axis bite arc, and retraction inertia. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body", meta=(ClampMin="1", ClampMax="12"))
    int32 NeckSegments = 3;

    /** Movement modes from the schema locomotion enum (ground, powered_flight, glide, ...). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
    TArray<FName> LocomotionModes;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Resources")
    FDragonResourceBaseline Resources;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
    FDragonAssessment BaseAssessment;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
    FDragonAIPersonality AIPersonality;

    /** Arena key -> affinity in [-1, 1]. Keys follow the schema's terrain_affinity object. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI")
    TMap<FName, float> TerrainAffinity;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0"))
    float GroundAcceleration = 450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0"))
    float FlightThrust = 700.0f;

    /** Wing loading in kg/m2. Higher means faster and less agile. */
    UFUNCTION(BlueprintPure, Category="Dragon|Movement")
    float GetWingLoading() const;

    UFUNCTION(BlueprintCallable, Category="Dragon|Validation")
    bool ValidateDefinition(TArray<FText>& OutErrors) const;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
