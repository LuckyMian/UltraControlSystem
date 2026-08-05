// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UCS_GameInstanceSubsystem.h"
#include "../Support/UCS_LogicStruct.h"
#include "../Support/UCS_Settings.h"
#include "Dom/JsonObject.h"
#include "UltraControlSystem/Slate/UCS_MenuBase.h"
#include "UCS_FunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ULTRACONTROLSYSTEM_API UUCS_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable,meta = (WorldContext = "WorldContextObject"))
	static UUCS_GameInstanceSubsystem* Get_UCS_GameInstanceSubsystem(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable,meta = (WorldContext = "WorldContextObject"))
	static UUCS_GlobalSettings* Get_UCS_GlobalSettings(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable)
	static FJsonObjectWrapper ReadGlobalSettingsJson();
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static UUCS_MenuBase* Load_UCS_UMG_ByConfig(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Ultra Control System|Logs")
	static void PrintUCSLog(FUCS_LogicStruct LogicInput, UObject* Object, FString Type);
};


UENUM(BlueprintType)
enum class ECameraType : uint8
{
	Flash     UMETA(DisplayName = "Flash"),
	Lerp     UMETA(DisplayName = "Lerp"),
};
