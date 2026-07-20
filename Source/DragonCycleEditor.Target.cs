using UnrealBuildTool;
using System.Collections.Generic;

public class DragonCycleEditorTarget : TargetRules
{
    public DragonCycleEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("DragonCycle");
        ExtraModuleNames.Add("DragonCycleEditor");
    }
}
