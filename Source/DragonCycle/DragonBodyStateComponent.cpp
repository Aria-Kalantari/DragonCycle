#include "DragonBodyStateComponent.h"
#include "DragonSpeciesDataAsset.h"

UDragonBodyStateComponent::UDragonBodyStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDragonBodyStateComponent::InitializeFromSpecies(const UDragonSpeciesDataAsset* SpeciesData)
{
    PartStates.Reset();
    PartArmor.Reset();
    PartImpairmentThreshold.Reset();

    if (!ensureMsgf(IsValid(SpeciesData), TEXT("InitializeFromSpecies requires valid species data.")))
    {
        return;
    }

    for (const FDragonBodyPartProfile& Profile : SpeciesData->BodyParts)
    {
        FDragonBodyPartState State;
        State.CurrentIntegrity = Profile.MaxIntegrity;
        State.MaxIntegrity = Profile.MaxIntegrity;
        State.bImpaired = false;

        PartStates.Add(Profile.Part, State);
        PartArmor.Add(Profile.Part, FMath::Clamp(Profile.Armor, 0.0f, 1.0f));
        PartImpairmentThreshold.Add(Profile.Part, FMath::Clamp(Profile.ImpairmentThreshold, 0.0f, 1.0f));
    }
}

FDragonDamageResult UDragonBodyStateComponent::ApplyLocalizedDamage(EDragonBodyPart Part, const FDragonLocalizedDamage& Damage)
{
    FDragonDamageResult Result;
    FDragonBodyPartState* State = PartStates.Find(Part);
    if (!State)
    {
        return Result;
    }

    const float Armor = PartArmor.FindRef(Part);
    const float EffectiveArmor = FMath::Clamp(Armor * (1.0f - Damage.Penetration), 0.0f, 1.0f);
    const float AppliedDamage = FMath::Max(0.0f, Damage.RawDamage * (1.0f - EffectiveArmor));
    const bool bWasImpaired = State->bImpaired;

    State->CurrentIntegrity = FMath::Clamp(State->CurrentIntegrity - AppliedDamage, 0.0f, State->MaxIntegrity);
    const float Threshold = PartImpairmentThreshold.FindRef(Part);
    State->bImpaired = State->GetIntegrityRatio() <= Threshold;

    Result.AppliedIntegrityDamage = AppliedDamage;
    Result.RemainingIntegrityRatio = State->GetIntegrityRatio();
    Result.bNewlyImpaired = !bWasImpaired && State->bImpaired;

    OnBodyPartChanged.Broadcast(Part, *State);
    return Result;
}

bool UDragonBodyStateComponent::GetBodyPartState(EDragonBodyPart Part, FDragonBodyPartState& OutState) const
{
    const FDragonBodyPartState* State = PartStates.Find(Part);
    if (!State)
    {
        return false;
    }

    OutState = *State;
    return true;
}

float UDragonBodyStateComponent::GetCapabilityMultiplier(EDragonBodyPart Part) const
{
    const FDragonBodyPartState* State = PartStates.Find(Part);
    if (!State)
    {
        return 0.0f;
    }

    const float Ratio = State->GetIntegrityRatio();
    return FMath::Clamp(FMath::SmoothStep(0.0f, 1.0f, Ratio), 0.1f, 1.0f);
}
