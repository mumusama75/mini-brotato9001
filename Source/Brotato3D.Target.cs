using UnrealBuildTool;
using System.Collections.Generic;

public class Brotato3DTarget : TargetRules
{
	public Brotato3DTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Brotato3D");
	}
}
