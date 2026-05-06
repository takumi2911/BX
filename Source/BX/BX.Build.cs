// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BX : ModuleRules
{
	public BX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags", "Niagara", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Json", "JsonUtilities" });

		// Slate UI (UMG / UserWidget 使用のため有効化)
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
