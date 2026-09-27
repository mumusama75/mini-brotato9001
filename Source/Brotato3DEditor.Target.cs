using UnrealBuildTool;
using System.Collections.Generic;

public class Brotato3DEditorTarget : TargetRules
{
	public Brotato3DEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Brotato3D");
	}
}
