using UnrealBuildTool;
using System.IO;

public class AirSimRoutePlanner : ModuleRules
{
	public AirSimRoutePlanner(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
    
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AirSim", "UMG"  });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "AirSim", "Json", "JsonUtilities" });

        PublicIncludePaths.AddRange(new string[] { ModuleDirectory });
//             ModuleDirectory + "/Data",
//             ModuleDirectory + "/UI",
//             ModuleDirectory + "/Utils",
//             ModuleDirectory + "/Actors"
//         });
    }
}
