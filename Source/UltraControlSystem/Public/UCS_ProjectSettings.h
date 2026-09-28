#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UCS_ProjectSettings.generated.h"

UENUM()
enum class EUCSLogicMapTarget : uint8
{
	InfoMap,
	LogicMap
};

/** Editor metadata for a string field stored in FUCS_LogicStruct. */
USTRUCT()
struct ULTRACONTROLSYSTEM_API FUCSLogicFieldDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Config, Category="Field")
	EUCSLogicMapTarget Target = EUCSLogicMapTarget::InfoMap;

	UPROPERTY(EditAnywhere, Config, Category="Field")
	FName Name;

	UPROPERTY(EditAnywhere, Config, Category="Field")
	FString DefaultValue;

	UPROPERTY(EditAnywhere, Config, Category="Field")
	bool bRequired = false;

	UPROPERTY(EditAnywhere, Config, Category="Field")
	TArray<FString> AllowedValues;

	UPROPERTY(EditAnywhere, Config, Category="Field", meta=(MultiLine=true))
	FText Description;
};

/** Runtime-safe UCS project settings. The legacy editor-only config section remains readable. */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Ultra Control System"))
class ULTRACONTROLSYSTEM_API UUCS_ProjectSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UUCS_ProjectSettings();

	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }

	UPROPERTY(EditAnywhere, Config, Category="Menu")
	bool bCreateMenuWidget = true;

	UPROPERTY(EditAnywhere, Config, Category="Menu", meta=(MetaClass="/Script/UltraControlSystem.UCS_MenuBase"))
	FSoftClassPath MenuWidgetClass;

	UPROPERTY(EditAnywhere, Config, Category="Diagnostics")
	bool bDebugPrint = false;

	UPROPERTY(EditAnywhere, Config, Category="Logic Schema")
	TArray<FUCSLogicFieldDefinition> LogicSchema;

	static bool IsMenuWidgetEnabled();
	static bool IsDebugPrintEnabled();
	static FString GetConfiguredMenuWidgetPath();
};
