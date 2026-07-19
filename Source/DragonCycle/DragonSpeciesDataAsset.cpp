#include "DragonSpeciesDataAsset.h"

bool UDragonSpeciesDataAsset::ValidateDefinition(TArray<FText>& OutErrors) const
{
    OutErrors.Reset();

    if (SpeciesId.IsNone())
    {
        OutErrors.Add(FText::FromString(TEXT("SpeciesId must be set.")));
    }

    if (MassKg <= 0.0f)
    {
        OutErrors.Add(FText::FromString(TEXT("MassKg must be positive.")));
    }

    if (WingAreaM2 <= 0.0f && LaunchMode != EDragonLaunchMode::None)
    {
        OutErrors.Add(FText::FromString(TEXT("A morphotype with a launch mode needs positive WingAreaM2.")));
    }

    if (BodyParts.IsEmpty())
    {
        OutErrors.Add(FText::FromString(TEXT("At least one body-part profile is required.")));
    }

    TSet<EDragonBodyPart> SeenParts;
    for (const FDragonBodyPartProfile& Profile : BodyParts)
    {
        if (SeenParts.Contains(Profile.Part))
        {
            OutErrors.Add(FText::FromString(TEXT("Body-part profiles must be unique.")));
            break;
        }
        SeenParts.Add(Profile.Part);
    }

    return OutErrors.IsEmpty();
}

float UDragonSpeciesDataAsset::GetWingLoading() const
{
    return WingAreaM2 > KINDA_SMALL_NUMBER ? MassKg / WingAreaM2 : 0.0f;
}

FPrimaryAssetId UDragonSpeciesDataAsset::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(TEXT("DragonSpecies"), SpeciesId);
}
