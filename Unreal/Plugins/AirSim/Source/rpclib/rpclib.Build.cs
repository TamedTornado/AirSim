// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class rpclib : ModuleRules
{
	public rpclib(ReadOnlyTargetRules Target) : base(Target)
	{
        Type = ModuleType.External;

        PublicDefinitions.Add("WITH_RPC_BINDING=1");

        // Add any include paths for the plugin
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

        string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Mac) ? "x64" : "x86";
        string ConfigurationString = (Target.Configuration == UnrealTargetConfiguration.Debug) ? "Debug" : "Release";

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", PlatformString, ConfigurationString, "rpc.lib"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux || Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "librpc.a"));
        }
    }
}
