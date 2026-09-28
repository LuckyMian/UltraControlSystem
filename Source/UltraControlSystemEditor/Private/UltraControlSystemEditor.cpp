#include "UltraControlSystemEditor.h"
#include "PropertyEditorModule.h"
#include "UCS_CameraDetails.h"
#include "../../UltraControlSystem/Actor/UCS_ActorBase.h"
#include "Framework/Docking/TabManager.h"
#include "MessageLogModule.h"
#include "SUCSWorkbench.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FUltraControlSystemEditorModule"

namespace
{
const FName UCSWorkbenchTabName(TEXT("UCSWorkbench"));
}

void FUltraControlSystemEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(
		AUCS_ActorBase::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FUCS_CameraDetails::MakeInstance)
	);

	PropertyModule.NotifyCustomizationModuleChanged();

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		UCSWorkbenchTabName,
		FOnSpawnTab::CreateRaw(this, &FUltraControlSystemEditorModule::SpawnWorkbenchTab))
		.SetDisplayName(LOCTEXT("WorkbenchTabTitle", "UCS Workbench"))
		.SetTooltipText(LOCTEXT("WorkbenchTabTooltip", "Configure and validate UCS cameras and scene widgets."))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUltraControlSystemEditorModule::RegisterMenus));

	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>(TEXT("MessageLog"));
	MessageLogModule.RegisterLogListing(TEXT("UCS"), LOCTEXT("MessageLogLabel", "UCS Validation"));
	
}

void FUltraControlSystemEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(UCSWorkbenchTabName);
	if (FModuleManager::Get().IsModuleLoaded(TEXT("MessageLog")))
	{
		FModuleManager::GetModuleChecked<FMessageLogModule>(TEXT("MessageLog")).UnregisterLogListing(TEXT("UCS"));
	}

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(AUCS_ActorBase::StaticClass()->GetFName());
	}
}

void FUltraControlSystemEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	UToolMenu* WindowMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Window"));
	FToolMenuSection& Section = WindowMenu->FindOrAddSection(TEXT("WindowLayout"));
	Section.AddMenuEntry(
		TEXT("OpenUCSWorkbench"),
		LOCTEXT("OpenWorkbench", "UCS Workbench"),
		LOCTEXT("OpenWorkbenchTooltip", "Open the UCS camera and widget workbench."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([]
		{
			FGlobalTabmanager::Get()->TryInvokeTab(UCSWorkbenchTabName);
		})));
}

TSharedRef<SDockTab> FUltraControlSystemEditorModule::SpawnWorkbenchTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)[SNew(SUCSWorkbench)];
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUltraControlSystemEditorModule, UltraControlSystemEditor)
