#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DragonSpeciesImportCommandlet.generated.h"

/**
 * ADR-006 importer: Data/Dragons/*.json -> UDragonSpeciesDataAsset under
 * /Game/DragonCycle/Data. JSON is the authoring source and review surface;
 * the generated asset holds runtime authority.
 *
 * Generic over the schema: part ids, enum strings, and asset names are all
 * resolved by reflection and name normalization, never by per-species
 * branches. The schema's body_parts id enum and EDragonBodyPart must be a
 * bijection or the run fails before importing anything.
 *
 * Re-runnable: existing assets are updated in place so references stay
 * stable, and a file that fails extraction or ValidateDefinition is rejected
 * without writing anything. Returns non-zero when any file fails.
 *
 *   UnrealEditor-Cmd.exe DragonCycle.uproject -run=DragonSpeciesImport
 */
UCLASS()
class UDragonSpeciesImportCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    virtual int32 Main(const FString& Params) override;
};
