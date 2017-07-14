// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class VolumetricBreaking : ModuleRules
{
	public VolumetricBreaking(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent", "RawMesh" });

        PrivateDependencyModuleNames.AddRange(new string[] { "RawMesh" });

        PublicAdditionalLibraries.Add(@"C:/Program Files (x86)/Epic Games/projects/VolumetricBreaking/ThirdParty/voro++/libs/voro++.lib");

        PublicIncludePaths.Add(@"C:/Program Files (x86)/Epic Games/projects/VolumetricBreaking/ThirdParty/voro++/includes");
        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
