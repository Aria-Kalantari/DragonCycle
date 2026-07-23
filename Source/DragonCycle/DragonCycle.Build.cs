using UnrealBuildTool;

public class DragonCycle : ModuleRules
{
    public DragonCycle(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Flat module layout (no Public/ folder): export the module root so
        // dependents like DragonCycleEditor can include these headers.
        PublicIncludePaths.Add(ModuleDirectory);

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "Json"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "AIModule",
            "Niagara",
            "UMG"
        });
    }
}
