// Copyright YTSS 2023. All Rights Reserved.

using UnrealBuildTool;

public class UltraControlSubSystemEditor : ModuleRules
{
    public UltraControlSubSystemEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "AssetTools", "UltraControlSubSystem",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UltraControlSubSystem",
                "UnrealEd",
            }
        );
    }
}