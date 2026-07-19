#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "DragonPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UDragonAbilitySystemComponent;
class UDragonAttributeSet;
class UDragonBodyStateComponent;
class UDragonSpeciesDataAsset;

UCLASS()
class DRAGONCYCLE_API ADragonPawn : public APawn, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ADragonPawn();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    UFUNCTION(BlueprintPure, Category="Dragon")
    UDragonBodyStateComponent* GetBodyStateComponent() const { return BodyState; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dragon|Components")
    TObjectPtr<USkeletalMeshComponent> DragonMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dragon|Components")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dragon|Components")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dragon|Components")
    TObjectPtr<UDragonAbilitySystemComponent> AbilitySystem;

    UPROPERTY()
    TObjectPtr<UDragonAttributeSet> Attributes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dragon|Components")
    TObjectPtr<UDragonBodyStateComponent> BodyState;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dragon|Definition")
    TObjectPtr<UDragonSpeciesDataAsset> SpeciesData;
};
