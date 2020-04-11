// Copyright 2016-2017 Tefel. All Rights Reserved.

using UnrealBuildTool;

public class UnrealVoxel : ModuleRules
{

    public UnrealVoxel(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                //"CoreUObject",
                //"Engine",
                "Slate",
                "SlateCore",
                //"InputCore",
                "MoviePlayer",
                "ProceduralMeshComponent"
            }
        );
    }
}
