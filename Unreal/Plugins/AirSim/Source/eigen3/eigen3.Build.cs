// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class eigen3 : ModuleRules
{
	public eigen3(ReadOnlyTargetRules Target) : base(Target)
	{
        Type = ModuleType.External;

        // Add any include paths for the plugin
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
    }
}
