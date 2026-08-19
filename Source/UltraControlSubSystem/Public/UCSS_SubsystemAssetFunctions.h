// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define LOAD_ByPaths_UCSS_Subsystem(Paths) FUCSS_SubsystemAssetFunctions::LoadAllUCSS_SubsystemByPaths(Paths)

#define FIND_AllUCSSPluginsDirectory(PluginDirectorys) FUCSS_SubsystemAssetFunctions::FindAllPluginsDirectory(PluginDirectorys)


class ULTRACONTROLSUBSYSTEM_API FUCSS_SubsystemAssetFunctions
{
public:
	
	static void FindAllPluginsDirectory(TSet<FString>& PluginDirectorys);
	static void LoadSubsystemByPaths(UClass* SubsystemClass, const TSet<FString>& Paths);
	static void LoadAllUCSS_SubsystemByPaths(const TSet<FString>& Paths);
};
