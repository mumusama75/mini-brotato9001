using UnrealBuildTool;
using System.IO;

public class Brotato3D : ModuleRules
{
	public Brotato3D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] {
			ModuleDirectory
		});

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"Niagara", 
			"UMG", 
			"Slate", 
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
