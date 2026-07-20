using UnrealBuildTool;

public class DragonCycleEditor : ModuleRules
{
    public DragonCycleEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "DragonCycle"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "AssetRegistry",
            "Json",
            "UnrealEd"
        });
    }
}
