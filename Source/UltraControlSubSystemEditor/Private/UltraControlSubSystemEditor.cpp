// Copyright YTSS 2023. All Rights Reserved.

#include "UltraControlSubSystemEditor.h"

#include "AssetToolsModule.h"

#include "UCSS_AssetTypeActions_Subsystem.h"

#include "UCSS_LocalPlayerSubsystem.h"
#include "UCSS_TickableGameInstanceSubsystem.h"
#include "UCSS_TickableLocalPlayerSubsystem.h"
#include "UCSS_TickableWorldSubsystem.h"
#include "UCSS_WorldSubsystem.h"

#include "IAssetTools.h"

#define LOCTEXT_NAMESPACE "FUltraControlSubSystemEditorModule"

void FUltraControlSubSystemEditorModule::StartupModule()
{
	// GetAssetTool
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// Spawn AssetTypeActions
	ActionTypes.Append
	(
		{
			MakeShareable(new FUCSS_AssetTypeActions_SubsystemTemplate<UUCSS_GameInstanceSubsystem>()),
			MakeShareable(new FUCSS_AssetTypeActions_SubsystemTemplate<UUCSS_WorldSubsystem>()),
			MakeShareable(new FUCSS_AssetTypeActions_SubsystemTemplate<UUCSS_LocalPlayerSubsystem>()),
			MakeShareable(new FUCSS_AssetTypeActions_SubsystemTemplate<UUCSS_TickableGameInstanceSubsystem>()),
			MakeShareable(new FUCSS_AssetTypeActions_SubsystemTemplate<UUCSS_TickableWorldSubsystem>()),
			MakeShareable(new FUCSS_AssetTypeActions_SubsystemTemplate<UUCSS_TickableLocalPlayerSubsystem>())
		}
	);

	// Register AssetTypeAction
	for (auto Action : ActionTypes)
	{
		AssetTools.RegisterAssetTypeActions(Action.ToSharedRef());
	}
}

void FUltraControlSubSystemEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::Get().GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (auto Action : ActionTypes)
		{
			AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUltraControlSubSystemEditorModule, UltraControlSubSystemEditor)
