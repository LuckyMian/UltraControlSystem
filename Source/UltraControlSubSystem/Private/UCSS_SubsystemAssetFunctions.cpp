// Copyright YTSS 2023. All Rights Reserved.

#include "UCSS_SubsystemAssetFunctions.h"
#include "UCSS_GameInstanceSubsystem.h"
#include "UCSS_LocalPlayerSubsystem.h"
#include "UCSS_WorldSubsystem.h"
#include "UltraControlSubSystem/UCSS_SubsystemType.h"
#include "Engine/ObjectLibrary.h"
#include "Interfaces/IPluginManager.h"

void FUCSS_SubsystemAssetFunctions::FindAllPluginsDirectory(TSet<FString>& PluginDirectorys)
{
	UE_LOG(LogUltraControlSubSystem, Log, TEXT("Begin to scan UCSS_Subsystem Asset"))
	IPluginManager& PluginManager = IPluginManager::Get();

	auto Plugins = PluginManager.GetEnabledPluginsWithContent();
	TSet<FString> PluginNames;

	for (auto& Plugin : Plugins)
	{
		if (Plugin->GetBaseDir().Contains("Marketplace") || Plugin->GetLoadedFrom() == EPluginLoadedFrom::Project)
		{
			const FString MountedPath = Plugin->GetMountedAssetPath();
			if (!MountedPath.IsEmpty())
			{
				PluginNames.Add(MountedPath);
			}
		}
	}

	PluginDirectorys.Add("/Game/");
	PluginDirectorys.Append(PluginNames);
	UE_LOG(LogUltraControlSubSystem, Log, TEXT("End of scan UCSS_Subsystem Asset"))
}

void FUCSS_SubsystemAssetFunctions::LoadSubsystemByPaths(UClass* SubsystemClass, const TSet<FString>& Paths)
{
	UE_LOG(LogUltraControlSubSystem, Log, TEXT("Begin to load all of %s Asset"), *SubsystemClass->GetName())

	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(SubsystemClass, true, GIsEditor);
	ObjectLibrary->AddToRoot();
	if (!IsValid(ObjectLibrary))
	{
		UE_LOG(LogUltraControlSubSystem, Warning,
		       TEXT("Unable to create UCSS_Subsystem ObjectLibrary. Cause the SubBlueprint of UCSS_... cant be uesd"
		       ));
		return;
	}
	ObjectLibrary->LoadBlueprintAssetDataFromPaths(Paths.Array());
	ObjectLibrary->LoadAssetsFromAssetData();
	// ObjectLibrary->LoadBlueprintsFromPaths(Paths.Array());

	UE_LOG(LogUltraControlSubSystem, Log, TEXT("End of load all of %s Asset"), *SubsystemClass->GetName())
}

void FUCSS_SubsystemAssetFunctions::LoadAllUCSS_SubsystemByPaths(const TSet<FString>& Paths)
{
	UE_LOG(LogUltraControlSubSystem, Log, TEXT("Begin to load all of UCSS_Subsystem Asset"))
	LoadSubsystemByPaths(UUCSS_GameInstanceSubsystem::StaticClass(), Paths);
	LoadSubsystemByPaths(UUCSS_LocalPlayerSubsystem::StaticClass(), Paths);
	LoadSubsystemByPaths(UUCSS_WorldSubsystem::StaticClass(), Paths);

	// LoadSubsystemByPaths(UUCSS_EngineSubsystem::StaticClass(), Paths);
	// LoadSubsystemByPaths(UCSS_EditorSubsystem::StaticClass(), Paths);

	// UAssetManager::Get().
	// 	ScanPathsForPrimaryAssets(PrimaryAssetType_UCSS_Subsystem, Paths.Array(),
	// 	                          UUCSS_GameInstanceSubsystem::StaticClass(), true, false, true);
	// UAssetManager::Get().
	// 	ScanPathsForPrimaryAssets(PrimaryAssetType_UCSS_Subsystem, Paths.Array(),
	// 	                          UUCSS_LocalPlayerSubsystem::StaticClass(), true, false, true);
	// UAssetManager::Get().
	// 	ScanPathsForPrimaryAssets(PrimaryAssetType_UCSS_Subsystem, Paths.Array(),
	// 	                          UUCSS_WorldSubsystem::StaticClass(), true, false, true);

	UE_LOG(LogUltraControlSubSystem, Log, TEXT("End of load All of UCSS_Subsystem Asset"))
}
