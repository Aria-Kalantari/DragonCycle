#include "DragonSpeciesImportCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "DragonSpeciesDataAsset.h"
#include "DragonTypes.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

DEFINE_LOG_CATEGORY_STATIC(LogDragonImport, Log, All);

namespace DragonImport
{

const TCHAR* ContentRoot = TEXT("/Game/DragonCycle/Data");

/** "wing_left" and "WingLeft" agree after lowercasing and dropping underscores. */
FString Normalize(const FString& Value)
{
    FString Out = Value.ToLower();
    Out.ReplaceInline(TEXT("_"), TEXT(""));
    return Out;
}

/** "prime_adult" -> "PrimeAdult". Drives asset naming from JSON fields alone. */
FString CamelCase(const FString& Value)
{
    TArray<FString> Tokens;
    Value.ParseIntoArray(Tokens, TEXT("_"));
    FString Out;
    for (const FString& Token : Tokens)
    {
        Out += FChar::ToUpper(Token[0]);
        Out += Token.Mid(1);
    }
    return Out;
}

/** Schema string -> UEnum value by normalized name. Generic over any project enum. */
template <typename TEnum>
bool ParseEnumValue(const FString& Value, const FString& Context, TEnum& OutValue, TArray<FString>& Errors)
{
    const UEnum* Enum = StaticEnum<TEnum>();
    const FString Target = Normalize(Value);
    for (int32 Index = 0; Index < Enum->NumEnums() - 1; ++Index)
    {
        if (Normalize(Enum->GetNameStringByIndex(Index)) == Target)
        {
            OutValue = static_cast<TEnum>(Enum->GetValueByIndex(Index));
            return true;
        }
    }
    Errors.Add(FString::Printf(TEXT("%s: '%s' is not a value of %s"), *Context, *Value, *Enum->GetName()));
    return false;
}

TSharedPtr<FJsonObject> LoadJsonObject(const FString& Path, TArray<FString>& Errors)
{
    FString Raw;
    if (!FFileHelper::LoadFileToString(Raw, *Path))
    {
        Errors.Add(FString::Printf(TEXT("%s: cannot read file"), *FPaths::GetCleanFilename(Path)));
        return nullptr;
    }
    TSharedPtr<FJsonObject> Object;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Raw);
    if (!FJsonSerializer::Deserialize(Reader, Object) || !Object.IsValid())
    {
        Errors.Add(FString::Printf(TEXT("%s: invalid JSON: %s"),
            *FPaths::GetCleanFilename(Path), *Reader->GetErrorMessage()));
        return nullptr;
    }
    return Object;
}

double RequireNumber(const TSharedPtr<FJsonObject>& Object, const TCHAR* Field, const FString& Context, TArray<FString>& Errors)
{
    double Value = 0.0;
    if (!Object.IsValid() || !Object->TryGetNumberField(FString(Field), Value))
    {
        Errors.Add(FString::Printf(TEXT("%s: missing or non-numeric '%s'"), *Context, Field));
    }
    return Value;
}

FString RequireString(const TSharedPtr<FJsonObject>& Object, const TCHAR* Field, const FString& Context, TArray<FString>& Errors)
{
    FString Value;
    if (!Object.IsValid() || !Object->TryGetStringField(FString(Field), Value))
    {
        Errors.Add(FString::Printf(TEXT("%s: missing or non-string '%s'"), *Context, Field));
    }
    return Value;
}

TSharedPtr<FJsonObject> RequireObject(const TSharedPtr<FJsonObject>& Object, const TCHAR* Field, const FString& Context, TArray<FString>& Errors)
{
    const TSharedPtr<FJsonObject>* Value = nullptr;
    if (!Object.IsValid() || !Object->TryGetObjectField(FString(Field), Value))
    {
        Errors.Add(FString::Printf(TEXT("%s: missing or non-object '%s'"), *Context, Field));
        return nullptr;
    }
    return *Value;
}

/**
 * The schema's part-id enum and EDragonBodyPart are one closed set (ADR-006).
 * Builds the id -> enum map and fails on any entry either side does not have.
 */
bool BuildPartMap(const TSharedPtr<FJsonObject>& Schema, TMap<FString, EDragonBodyPart>& OutPartMap, TArray<FString>& Errors)
{
    TSharedPtr<FJsonObject> Cursor = Schema;
    for (const TCHAR* Step : { TEXT("properties"), TEXT("body_parts"), TEXT("items"), TEXT("properties"), TEXT("id") })
    {
        Cursor = RequireObject(Cursor, Step, TEXT("schema"), Errors);
        if (!Cursor.IsValid())
        {
            return false;
        }
    }
    const TArray<TSharedPtr<FJsonValue>>* EnumValues = nullptr;
    if (!Cursor->TryGetArrayField(TEXT("enum"), EnumValues))
    {
        Errors.Add(TEXT("schema: body_parts id has no enum array"));
        return false;
    }

    const UEnum* PartEnum = StaticEnum<EDragonBodyPart>();
    TMap<FString, EDragonBodyPart> ByNormalizedName;
    for (int32 Index = 0; Index < PartEnum->NumEnums() - 1; ++Index)
    {
        ByNormalizedName.Add(Normalize(PartEnum->GetNameStringByIndex(Index)),
                             static_cast<EDragonBodyPart>(PartEnum->GetValueByIndex(Index)));
    }

    TSet<EDragonBodyPart> Matched;
    for (const TSharedPtr<FJsonValue>& Value : *EnumValues)
    {
        FString Id;
        if (!Value->TryGetString(Id))
        {
            Errors.Add(TEXT("schema: non-string entry in body part id enum"));
            continue;
        }
        if (const EDragonBodyPart* Part = ByNormalizedName.Find(Normalize(Id)))
        {
            if (Matched.Contains(*Part))
            {
                Errors.Add(FString::Printf(TEXT("closed-set violation: schema part '%s' duplicates an earlier entry"), *Id));
                continue;
            }
            OutPartMap.Add(Id, *Part);
            Matched.Add(*Part);
        }
        else
        {
            Errors.Add(FString::Printf(TEXT("closed-set violation: schema part '%s' has no EDragonBodyPart entry"), *Id));
        }
    }
    for (const TPair<FString, EDragonBodyPart>& Pair : ByNormalizedName)
    {
        if (!Matched.Contains(Pair.Value))
        {
            Errors.Add(FString::Printf(TEXT("closed-set violation: EDragonBodyPart::%s is missing from the schema part-id enum"),
                *PartEnum->GetNameStringByValue(static_cast<int64>(Pair.Value))));
        }
    }
    return true;
}

bool ImportSpeciesFile(const FString& FilePath, const TMap<FString, EDragonBodyPart>& PartMap, TArray<FString>& Errors)
{
    const FString Label = FPaths::GetCleanFilename(FilePath);
    TArray<FString> E;

    const TSharedPtr<FJsonObject> Json = LoadJsonObject(FilePath, E);
    if (!Json.IsValid())
    {
        Errors.Append(E);
        return false;
    }

    // ------------------------------------------------------------ extraction
    const FString Id = RequireString(Json, TEXT("id"), Label, E);
    const FString MorphotypeName = RequireString(Json, TEXT("morphotype"), Label, E);
    const FString LifeStageName = RequireString(Json, TEXT("life_stage"), Label, E);
    const FString ExperienceName = RequireString(Json, TEXT("experience"), Label, E);
    const FString DisplayName = RequireString(Json, TEXT("display_name"), Label, E);
    const FString CombatIdentity = RequireString(Json, TEXT("combat_identity"), Label, E);

    EDragonMorphotype Morphotype = EDragonMorphotype::Drake;
    EDragonLifeStage LifeStage = EDragonLifeStage::PrimeAdult;
    EDragonExperienceRank Experience = EDragonExperienceRank::Seasoned;
    if (!MorphotypeName.IsEmpty()) { ParseEnumValue(MorphotypeName, Label, Morphotype, E); }
    if (!LifeStageName.IsEmpty()) { ParseEnumValue(LifeStageName, Label, LifeStage, E); }
    if (!ExperienceName.IsEmpty()) { ParseEnumValue(ExperienceName, Label, Experience, E); }

    double Condition = 1.0;
    Json->TryGetNumberField(TEXT("condition"), Condition); // optional in the schema

    FDragonBodyPlan Plan;
    int32 NeckSegments = 1;
    if (const TSharedPtr<FJsonObject> BodyPlan = RequireObject(Json, TEXT("body_plan"), Label, E))
    {
        Plan.Legs = static_cast<int32>(RequireNumber(BodyPlan, TEXT("legs"), Label, E));
        Plan.IndependentWings = static_cast<int32>(RequireNumber(BodyPlan, TEXT("independent_wings"), Label, E));
        Plan.WingArms = static_cast<int32>(RequireNumber(BodyPlan, TEXT("wing_arms"), Label, E));
        Plan.Heads = static_cast<int32>(RequireNumber(BodyPlan, TEXT("heads"), Label, E));
        BodyPlan->TryGetBoolField(TEXT("serpentine"), Plan.bSerpentine);
        NeckSegments = static_cast<int32>(RequireNumber(BodyPlan, TEXT("neck_segments"), Label, E));
    }

    TArray<FName> LocomotionModes;
    const TArray<TSharedPtr<FJsonValue>>* LocomotionValues = nullptr;
    if (Json->TryGetArrayField(TEXT("locomotion"), LocomotionValues))
    {
        for (const TSharedPtr<FJsonValue>& Value : *LocomotionValues)
        {
            FString Mode;
            if (Value->TryGetString(Mode))
            {
                LocomotionModes.AddUnique(FName(*Mode));
            }
        }
    }
    else
    {
        E.Add(Label + TEXT(": missing 'locomotion'"));
    }

    TArray<FDragonBodyPartProfile> Parts;
    const TArray<TSharedPtr<FJsonValue>>* PartValues = nullptr;
    if (Json->TryGetArrayField(TEXT("body_parts"), PartValues))
    {
        for (const TSharedPtr<FJsonValue>& Value : *PartValues)
        {
            const TSharedPtr<FJsonObject>* PartObject = nullptr;
            if (!Value->TryGetObject(PartObject))
            {
                E.Add(Label + TEXT(": body_parts entry is not an object"));
                continue;
            }
            const FString PartId = RequireString(*PartObject, TEXT("id"), Label, E);
            const EDragonBodyPart* Mapped = PartMap.Find(PartId);
            if (!Mapped)
            {
                E.Add(FString::Printf(TEXT("%s: body part '%s' is not in the closed part set"), *Label, *PartId));
                continue;
            }
            FDragonBodyPartProfile Profile;
            Profile.Part = *Mapped;
            Profile.MaxIntegrity = static_cast<float>(RequireNumber(*PartObject, TEXT("max_integrity"), Label, E));
            Profile.Armor = static_cast<float>(RequireNumber(*PartObject, TEXT("armor"), Label, E));
            Profile.ImpairmentThreshold = static_cast<float>(RequireNumber(*PartObject, TEXT("break_threshold"), Label, E));
            Parts.Add(Profile);
        }
    }
    else
    {
        E.Add(Label + TEXT(": missing 'body_parts'"));
    }

    FDragonResourceBaseline Resources;
    if (const TSharedPtr<FJsonObject> ResourcesJson = RequireObject(Json, TEXT("resources"), Label, E))
    {
        Resources.Health = static_cast<float>(RequireNumber(ResourcesJson, TEXT("health"), Label, E));
        Resources.Stamina = static_cast<float>(RequireNumber(ResourcesJson, TEXT("stamina"), Label, E));
        Resources.HeatCapacity = static_cast<float>(RequireNumber(ResourcesJson, TEXT("heat_capacity"), Label, E));
        Resources.BreathReserve = static_cast<float>(RequireNumber(ResourcesJson, TEXT("breath_reserve"), Label, E));
        Resources.Balance = static_cast<float>(RequireNumber(ResourcesJson, TEXT("balance"), Label, E));
    }

    double MassKg = 0.0, WingAreaM2 = 0.0, MaxTurnRate = 0.0, StallSpeed = 0.0;
    EDragonLaunchMode LaunchMode = EDragonLaunchMode::None;
    if (const TSharedPtr<FJsonObject> Envelope = RequireObject(Json, TEXT("flight_envelope"), Label, E))
    {
        MassKg = RequireNumber(Envelope, TEXT("mass_kg"), Label, E);
        WingAreaM2 = RequireNumber(Envelope, TEXT("wing_area_m2"), Label, E);
        MaxTurnRate = RequireNumber(Envelope, TEXT("max_turn_rate_deg_s"), Label, E);
        StallSpeed = RequireNumber(Envelope, TEXT("stall_speed_ms"), Label, E);
        const FString LaunchModeName = RequireString(Envelope, TEXT("launch_mode"), Label, E);
        if (!LaunchModeName.IsEmpty()) { ParseEnumValue(LaunchModeName, Label, LaunchMode, E); }
    }

    FDragonAssessment Assessment;
    FDragonAIPersonality Personality;
    if (const TSharedPtr<FJsonObject> Weights = RequireObject(Json, TEXT("ai_weights"), Label, E))
    {
        Assessment.SelfBias = static_cast<float>(RequireNumber(Weights, TEXT("rhp_self_bias"), Label, E));
        Assessment.ResourceValue = static_cast<float>(RequireNumber(Weights, TEXT("resource_value"), Label, E));
        Assessment.Persistence = static_cast<float>(RequireNumber(Weights, TEXT("persistence"), Label, E));
        Assessment.MutualAssessmentWeight = static_cast<float>(RequireNumber(Weights, TEXT("mutual_assessment_weight"), Label, E));
        Assessment.Skill = static_cast<float>(RequireNumber(Weights, TEXT("skill"), Label, E));
        Personality.GroundAggression = static_cast<float>(RequireNumber(Weights, TEXT("ground_aggression"), Label, E));
        Personality.AirAggression = static_cast<float>(RequireNumber(Weights, TEXT("air_aggression"), Label, E));
        Personality.Grapple = static_cast<float>(RequireNumber(Weights, TEXT("grapple"), Label, E));
        Personality.InjuryProtection = static_cast<float>(RequireNumber(Weights, TEXT("injury_protection"), Label, E));
        Personality.DisplayPreference = static_cast<float>(RequireNumber(Weights, TEXT("display_preference"), Label, E));
        Personality.TerrainExploitation = static_cast<float>(RequireNumber(Weights, TEXT("terrain_exploitation"), Label, E));
    }

    TMap<FName, float> TerrainAffinity;
    if (const TSharedPtr<FJsonObject> Terrain = RequireObject(Json, TEXT("terrain_affinity"), Label, E))
    {
        for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : Terrain->Values)
        {
            double Affinity = 0.0;
            if (Pair.Value.IsValid() && Pair.Value->TryGetNumber(Affinity))
            {
                TerrainAffinity.Add(FName(*Pair.Key), static_cast<float>(Affinity));
            }
            else
            {
                E.Add(FString::Printf(TEXT("%s: terrain_affinity '%s' is not a number"), *Label, *Pair.Key));
            }
        }
    }

    if (!E.IsEmpty())
    {
        Errors.Append(E);
        return false; // nothing touched on disk
    }

    // Populate a transient candidate first, so a file that fails validation
    // never leaves a half-written asset behind on disk.
    UDragonSpeciesDataAsset* Candidate = NewObject<UDragonSpeciesDataAsset>(
        GetTransientPackage(), NAME_None, RF_Transient);

    // Schema-backed fields only. Tuning fields without a JSON source
    // (GroundAcceleration, FlightThrust) are deliberately left untouched.
    Candidate->SpeciesId = FName(*Id);
    Candidate->DisplayName = FText::FromString(DisplayName);
    Candidate->Morphotype = Morphotype;
    Candidate->LifeStage = LifeStage;
    Candidate->Experience = Experience;
    Candidate->Condition = static_cast<float>(Condition);
    Candidate->CombatIdentity = CombatIdentity;
    Candidate->BodyParts = Parts;
    Candidate->BodyPlan = Plan;
    Candidate->NeckSegments = NeckSegments;
    Candidate->LocomotionModes = LocomotionModes;
    Candidate->Resources = Resources;
    Candidate->MassKg = static_cast<float>(MassKg);
    Candidate->WingAreaM2 = static_cast<float>(WingAreaM2);
    Candidate->LaunchMode = LaunchMode;
    Candidate->MaxTurnRateDegPerSecond = static_cast<float>(MaxTurnRate);
    Candidate->StallSpeedMetersPerSecond = static_cast<float>(StallSpeed);
    Candidate->BaseAssessment = Assessment;
    Candidate->AIPersonality = Personality;
    Candidate->TerrainAffinity = TerrainAffinity;

    TArray<FText> ValidationErrors;
    if (!Candidate->ValidateDefinition(ValidationErrors))
    {
        for (const FText& Error : ValidationErrors)
        {
            Errors.Add(FString::Printf(TEXT("%s: %s"), *Label, *Error.ToString()));
        }
        return false; // reject rather than write a partial asset
    }

    // ------------------------------------------------- destination asset
    const FString AssetName = FString::Printf(TEXT("DA_%s_%s"), *CamelCase(MorphotypeName), *CamelCase(LifeStageName));
    const FString PackageName = FString(ContentRoot) / AssetName;

    UPackage* Package = LoadPackage(nullptr, *PackageName, LOAD_NoWarn | LOAD_Quiet);
    UDragonSpeciesDataAsset* Asset = nullptr;
    bool bNewAsset = false;
    if (Package)
    {
        if (UObject* Existing = FindObject<UObject>(Package, *AssetName))
        {
            Asset = Cast<UDragonSpeciesDataAsset>(Existing);
            if (!Asset)
            {
                Errors.Add(FString::Printf(TEXT("%s: %s already exists as %s, refusing to overwrite"),
                    *Label, *PackageName, *Existing->GetClass()->GetName()));
                return false;
            }
        }
    }
    if (!Asset)
    {
        if (!Package)
        {
            Package = CreatePackage(*PackageName);
        }
        Asset = NewObject<UDragonSpeciesDataAsset>(Package, *AssetName, RF_Public | RF_Standalone);
        bNewAsset = true;
    }

    // Property-driven rather than field-by-field, so a new UPROPERTY is picked
    // up without editing this loop. Saving an unchanged package would rewrite
    // header GUIDs and produce a fresh LFS blob for no content change, so an
    // identical asset is left alone.
    bool bChanged = bNewAsset;
    for (TFieldIterator<FProperty> It(UDragonSpeciesDataAsset::StaticClass(),
             EFieldIteratorFlags::ExcludeSuper); It; ++It)
    {
        if (!It->Identical_InContainer(Candidate, Asset))
        {
            It->CopyCompleteValue_InContainer(Asset, Candidate);
            bChanged = true;
        }
    }

    if (!bChanged)
    {
        UE_LOG(LogDragonImport, Display, TEXT("[M06] %s -> %s unchanged, not rewritten"), *Label, *PackageName);
        return true;
    }

    Asset->MarkPackageDirty();
    if (bNewAsset)
    {
        FAssetRegistryModule::AssetCreated(Asset);
    }

    const FString FileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    if (!UPackage::SavePackage(Package, Asset, *FileName, SaveArgs))
    {
        Errors.Add(FString::Printf(TEXT("%s: failed to save %s"), *Label, *PackageName));
        return false;
    }

    UE_LOG(LogDragonImport, Display, TEXT("[M06] %s -> %s (%d parts, wing loading %.1f kg/m2)"),
        *Label, *PackageName, Asset->BodyParts.Num(), Asset->GetWingLoading());
    return true;
}

} // namespace DragonImport

int32 UDragonSpeciesImportCommandlet::Main(const FString& Params)
{
    TArray<FString> Errors;

    const FString SchemaPath = FPaths::ProjectDir() / TEXT("Data/Schemas/dragon_species.schema.json");
    const TSharedPtr<FJsonObject> Schema = DragonImport::LoadJsonObject(SchemaPath, Errors);

    TMap<FString, EDragonBodyPart> PartMap;
    if (Schema.IsValid())
    {
        DragonImport::BuildPartMap(Schema, PartMap, Errors);
    }

    int32 Imported = 0;
    if (Errors.IsEmpty()) // a broken vocabulary poisons every file; do not import against it
    {
        const FString DragonsDir = FPaths::ProjectDir() / TEXT("Data/Dragons");
        TArray<FString> Files;
        IFileManager::Get().FindFiles(Files, *(DragonsDir / TEXT("*.json")), true, false);
        Files.Sort();
        if (Files.IsEmpty())
        {
            Errors.Add(TEXT("Data/Dragons contains no species JSON"));
        }
        for (const FString& File : Files)
        {
            if (DragonImport::ImportSpeciesFile(DragonsDir / File, PartMap, Errors))
            {
                ++Imported;
            }
        }
    }

    for (const FString& Error : Errors)
    {
        UE_LOG(LogDragonImport, Error, TEXT("[M06] %s"), *Error);
    }
    UE_LOG(LogDragonImport, Display, TEXT("[M06] imported %d species, %d error(s)"), Imported, Errors.Num());
    return Errors.IsEmpty() ? 0 : 1;
}
