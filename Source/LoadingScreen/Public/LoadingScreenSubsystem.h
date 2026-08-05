// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoadingSetting.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LoadScreenUserWidget.h"
#include "LoadingScreenSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class LOADINGSCREEN_API ULoadingScreenSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void OpenLevelWithLoadScreen(FName MapPackageName);
	UFUNCTION(BlueprintCallable, DisplayName="OpenLevelWithLoadScreen", Category="LSP")
	void K2_OpenLevelWithLoadScreen(TSoftObjectPtr<UWorld> Map, TSoftObjectPtr<UWorld> TransitionMap, TSoftClassPtr<ULoadScreenUserWidget> LoadScreenUserWidgetClass);

	float GetLoadPercent();
protected:

	void OnCheckInTransitionMap();
	
protected:
	UPROPERTY()
	ULoadingSetting* LoadingSetting;
	UPROPERTY()
	ULoadScreenUserWidget* LoadScreenUserWidget;

	FName LoadMapPackageName;
	
	
};
