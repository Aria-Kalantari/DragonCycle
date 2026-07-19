#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DragonAttributeSet.generated.h"

#define DRAGON_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DRAGONCYCLE_API UDragonAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UDragonAttributeSet();

    UPROPERTY(BlueprintReadOnly, Category="Dragon|Attributes")
    FGameplayAttributeData Health;
    DRAGON_ATTRIBUTE_ACCESSORS(UDragonAttributeSet, Health)

    UPROPERTY(BlueprintReadOnly, Category="Dragon|Attributes")
    FGameplayAttributeData MaxHealth;
    DRAGON_ATTRIBUTE_ACCESSORS(UDragonAttributeSet, MaxHealth)

    UPROPERTY(BlueprintReadOnly, Category="Dragon|Attributes")
    FGameplayAttributeData Stamina;
    DRAGON_ATTRIBUTE_ACCESSORS(UDragonAttributeSet, Stamina)

    UPROPERTY(BlueprintReadOnly, Category="Dragon|Attributes")
    FGameplayAttributeData MaxStamina;
    DRAGON_ATTRIBUTE_ACCESSORS(UDragonAttributeSet, MaxStamina)

    UPROPERTY(BlueprintReadOnly, Category="Dragon|Attributes")
    FGameplayAttributeData Heat;
    DRAGON_ATTRIBUTE_ACCESSORS(UDragonAttributeSet, Heat)

    UPROPERTY(BlueprintReadOnly, Category="Dragon|Attributes")
    FGameplayAttributeData MaxHeat;
    DRAGON_ATTRIBUTE_ACCESSORS(UDragonAttributeSet, MaxHeat)

    UPROPERTY(BlueprintReadOnly, Category="Dragon|Attributes")
    FGameplayAttributeData BreathReserve;
    DRAGON_ATTRIBUTE_ACCESSORS(UDragonAttributeSet, BreathReserve)

    UPROPERTY(BlueprintReadOnly, Category="Dragon|Attributes")
    FGameplayAttributeData Balance;
    DRAGON_ATTRIBUTE_ACCESSORS(UDragonAttributeSet, Balance)

    /**
     * Clamps CurrentValue for duration-based and infinite modifiers.
     *
     * PostGameplayEffectExecute only fires for instant and periodic effects, so
     * without this a sustained drain — sprint stamina, breath heat — pushes the
     * current value past its bound and never comes back. Stamina degradation
     * before zero is pillar 3, so this is load-bearing, not defensive coding.
     */
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
    /** Shared bounds for both clamp paths, so the two cannot drift apart. */
    void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};
