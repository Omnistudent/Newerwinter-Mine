// Copyright 2016-2017 Tefel. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealVoxelEditorTarget : TargetRules
{
    public UnrealVoxelEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
        ExtraModuleNames.AddRange(new string[] { "UnrealVoxel" });
    }
}
