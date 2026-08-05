#include "UltraControlSystemEditor.h"
#include "PropertyEditorModule.h"
#include "UCS_CameraDetails.h"
#include "../Actor/UCS_ActorBase.h"
#include "ISettingsModule.h"
#include "UCS_Config.h"

#define LOCTEXT_NAMESPACE "FUltraControlSystemEditorModule"

void FUltraControlSystemEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(
		AUCS_ActorBase::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FUCS_CameraDetails::MakeInstance)
	);

	PropertyModule.NotifyCustomizationModuleChanged();
	
	// Get Settings Module
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Register to Project Settings
		SettingsModule->RegisterSettings(
			"Project",         // ContainerName
			"Plugins",         // CategoryName
			"UCS", // DisplayName
			LOCTEXT("UCSSettingsName", "UCS"),
			LOCTEXT("UCS", "UCS"),
			GetMutableDefault<UUCS_Config>() // 配置对象
		);
	}
}

void FUltraControlSystemEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(AUCS_ActorBase::StaticClass()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUltraControlSystemEditorModule, UltraControlSystemEditor)
