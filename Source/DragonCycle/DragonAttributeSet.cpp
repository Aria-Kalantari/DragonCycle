#include "DragonAttributeSet.h"
#include "GameplayEffectExtension.h"

UDragonAttributeSet::UDragonAttributeSet()
{
    InitHealth(1000.0f);
    InitMaxHealth(1000.0f);
    InitStamina(100.0f);
    InitMaxStamina(100.0f);
    InitHeat(0.0f);
    InitMaxHeat(100.0f);
    InitBreathReserve(100.0f);
    InitBalance(100.0f);
}

void UDragonAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
    }
    else if (Attribute == GetHeatAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHeat());
    }
    else if (Attribute == GetBreathReserveAttribute())
    {
        NewValue = FMath::Max(0.0f, NewValue);
    }
    else if (Attribute == GetBalanceAttribute())
    {
        NewValue = FMath::Max(0.0f, NewValue);
    }
}

void UDragonAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UDragonAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    float NewValue = Data.EvaluatedData.Attribute.GetNumericValue(this);
    ClampAttribute(Data.EvaluatedData.Attribute, NewValue);
    Data.EvaluatedData.Attribute.SetNumericValueChecked(NewValue, this);
}
