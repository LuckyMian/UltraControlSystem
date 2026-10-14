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

	/**
	 * Keeps the latest UCS logic state per LogicMap["Key"] so receivers that spawn
	 * after the message was sent still receive it. Turning this off restores the
	 * original "live dispatch only" behavior.
	 */
	UPROPERTY(EditAnywhere, Config, Category="Message Queue")
	bool bEnableMessageQueue = true;

	/** Seconds a queued camera focus keeps retrying while the UCS Pawn has not spawned yet. */
	UPROPERTY(EditAnywhere, Config, Category="Message Queue", meta=(ClampMin="0.0", UIMin="0.0"))
	float MessageQueueRetrySeconds = 2.0f;

	/** Safety cap on retained keys; the oldest state is evicted when the queue grows past it. */
	UPROPERTY(EditAnywhere, Config, Category="Message Queue", meta=(ClampMin="1", UIMin="1"))
	int32 MaxPendingMessages = 32;

	UPROPERTY(EditAnywhere, Config, Category="Logic Schema")
	TArray<FUCSLogicFieldDefinition> LogicSchema;

	static bool IsMenuWidgetEnabled();
	static bool IsDebugPrintEnabled();
	static FString GetConfiguredMenuWidgetPath();
	static bool IsMessageQueueEnabled();
	static float GetMessageQueueRetrySeconds();
	static int32 GetMaxPendingMessages();
};
