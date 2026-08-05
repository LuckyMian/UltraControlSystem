#include "UCS_Settings.h"

#include "UCS_FunctionLibrary.h"


UUCS_GlobalSettings::UUCS_GlobalSettings()
{
	Json = UUCS_FunctionLibrary::ReadGlobalSettingsJson();
	GlobalSettings.GlobalMovementSpeed = Json.JsonObject->GetNumberField(TEXT("GlobalMovementSpeed"));
	GlobalSettings.GlobalRotationSpeed = Json.JsonObject->GetNumberField(TEXT("GlobalRotationSpeed"));
	GlobalSettings.GlobalZoomSpeed = Json.JsonObject->GetNumberField(TEXT("GlobalZoomSpeed"));
}
