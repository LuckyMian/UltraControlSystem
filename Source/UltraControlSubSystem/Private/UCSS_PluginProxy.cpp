// Copyright YTSS 2023. All Rights Reserved.


#include "UCSS_PluginProxy.h"
#include "UCSS_SubsystemAssetFunctions.h"
#include "Engine/AssetManager.h"

void UUCSS_PluginProxy::Initialize(FSubsystemCollectionBase& Collection)
{
	// UAssetManager::Get();
	
	TSet<FString> PreparedPaths;
	FIND_AllUCSSPluginsDirectory(PreparedPaths);
	LOAD_ByPaths_UCSS_Subsystem(PreparedPaths);
}
