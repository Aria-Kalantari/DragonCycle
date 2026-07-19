using UnrealBuildTool;
using System.Collections.Generic;

public class DragonCycleEditorTarget : TargetRules
{
    public DragonCycleEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("DragonCycle");
    }
}
