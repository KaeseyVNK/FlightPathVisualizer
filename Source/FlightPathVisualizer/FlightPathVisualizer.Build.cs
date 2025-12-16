// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FlightPathVisualizer : ModuleRules
{
	public FlightPathVisualizer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "DesktopPlatform", "ApplicationCore", "GeoReferencing", "UMG", "Slate", "SlateCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicDefinitions.Add("WITH_AUTOMATION_TEST=1");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
