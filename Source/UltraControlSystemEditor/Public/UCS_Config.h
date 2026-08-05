// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UltraControlSystem/Slate/UCS_WidgetBase.h"
#include "UCS_Config.generated.h"

/**
 * 
 */
UCLASS(config=Game, defaultconfig)
class ULTRACONTROLSYSTEMEDITOR_API UUCS_Config : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, config ,Category="UMG")
	FString WidgetPath = "/Script/UMGEditor.WidgetBlueprint'/UltraControlSystem/Menu/WBP_UCS_MenuBase.WBP_UCS_MenuBase'";
};
