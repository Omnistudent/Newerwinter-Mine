// Copyright 2016-2017 Tefel. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealVoxelTarget : TargetRules
{
	public UnrealVoxelTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
        ExtraModuleNames.AddRange(new string[] { "UnrealVoxel" });
    }
}
