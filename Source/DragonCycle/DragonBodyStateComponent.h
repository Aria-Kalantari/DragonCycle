#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DragonTypes.h"
#include "DragonBodyStateComponent.generated.h"

class UDragonSpeciesDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDragonBodyPartChanged, EDragonBodyPart, Part, const FDragonBodyPartState&, NewState);

UCLASS(ClassGroup=(Dragon), meta=(BlueprintSpawnableComponent))
class DRAGONCYCLE_API UDragonBodyStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDragonBodyStateComponent();

    UPROPERTY(BlueprintAssignable, Category="Dragon|Body")
    FDragonBodyPartChanged OnBodyPartChanged;

    UFUNCTION(BlueprintCallable, Category="Dragon|Body")
    void InitializeFromSpecies(const UDragonSpeciesDataAsset* SpeciesData);

    UFUNCTION(BlueprintCallable, Category="Dragon|Body")
    FDragonDamageResult ApplyLocalizedDamage(EDragonBodyPart Part, const FDragonLocalizedDamage& Damage);

    UFUNCTION(BlueprintPure, Category="Dragon|Body")
    bool GetBodyPartState(EDragonBodyPart Part, FDragonBodyPartState& OutState) const;

    UFUNCTION(BlueprintPure, Category="Dragon|Body")
    float GetCapabilityMultiplier(EDragonBodyPart Part) const;

private:
    UPROPERTY(VisibleInstanceOnly, Category="Dragon|Body")
    TMap<EDragonBodyPart, FDragonBodyPartState> PartStates;

    UPROPERTY(VisibleInstanceOnly, Category="Dragon|Body")
    TMap<EDragonBodyPart, float> PartArmor;

    UPROPERTY(VisibleInstanceOnly, Category="Dragon|Body")
    TMap<EDragonBodyPart, float> PartImpairmentThreshold;
};
