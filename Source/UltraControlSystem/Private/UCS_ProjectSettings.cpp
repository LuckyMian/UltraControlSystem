#include "UCS_ProjectSettings.h"

#include "Misc/ConfigCacheIni.h"

namespace
{
const TCHAR* NewSection = TEXT("/Script/UltraControlSystem.UCS_ProjectSettings");
const TCHAR* LegacySection = TEXT("/Script/UltraControlSystemEditor.UCS_Config");
}

UUCS_ProjectSettings::UUCS_ProjectSettings()
{
	MenuWidgetClass = FSoftClassPath(TEXT("/UltraControlSystem/Menu/WBP_UCS_MenuBase.WBP_UCS_MenuBase_C"));

	FUCSLogicFieldDefinition CnName;
	CnName.Target = EUCSLogicMapTarget::InfoMap;
	CnName.Name = TEXT("CnName");
	CnName.Description = NSLOCTEXT("UCS", "SchemaCnName", "Chinese display name.");
	LogicSchema.Add(CnName);

	FUCSLogicFieldDefinition EnName;
	EnName.Target = EUCSLogicMapTarget::InfoMap;
	EnName.Name = TEXT("EnName");
	EnName.Description = NSLOCTEXT("UCS", "SchemaEnName", "English display name.");
	LogicSchema.Add(EnName);

	FUCSLogicFieldDefinition Key;
	Key.Target = EUCSLogicMapTarget::LogicMap;
	Key.Name = TEXT("Key");
	Key.bRequired = true;
	Key.Description = NSLOCTEXT("UCS", "SchemaKey", "Unique camera routing key.");
	LogicSchema.Add(Key);
}

bool UUCS_ProjectSettings::IsMenuWidgetEnabled()
{
	bool Value = GetDefault<UUCS_ProjectSettings>()->bCreateMenuWidget;
	if (GConfig && GConfig->GetBool(NewSection, TEXT("bCreateMenuWidget"), Value, GGameIni))
	{
		return Value;
	}
	if (GConfig)
	{
		GConfig->GetBool(LegacySection, TEXT("bIsCreateWidget"), Value, GGameIni);
	}
	return Value;
}

bool UUCS_ProjectSettings::IsDebugPrintEnabled()
{
	bool Value = GetDefault<UUCS_ProjectSettings>()->bDebugPrint;
	if (GConfig && GConfig->GetBool(NewSection, TEXT("bDebugPrint"), Value, GGameIni))
	{
		return Value;
	}
	if (GConfig)
	{
		GConfig->GetBool(LegacySection, TEXT("bIsDebugPrint"), Value, GGameIni);
	}
	return Value;
}

FString UUCS_ProjectSettings::GetConfiguredMenuWidgetPath()
{
	FString Value;
	if (GConfig && GConfig->GetString(NewSection, TEXT("MenuWidgetClass"), Value, GGameIni) && !Value.IsEmpty())
	{
		return Value;
	}
	if (GConfig && GConfig->GetString(LegacySection, TEXT("WidgetPath"), Value, GGameIni) && !Value.IsEmpty())
	{
		return Value;
	}
	return GetDefault<UUCS_ProjectSettings>()->MenuWidgetClass.ToString();
}
