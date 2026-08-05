// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LoadScreenUserWidget.h"
#include "LoadingSetting.generated.h"

/**
 * 
 */
UCLASS()
class LOADINGSCREEN_API ULoadingSetting : public UObject
{
	GENERATED_BODY()
public:
	//配置切换过度地图，尽量是空白地图
	UPROPERTY(EditAnywhere, Category="UCS")
	TSoftObjectPtr<UWorld> TransitionMap;
	//配置切换过度UI，用于展示过度进度信息，需要重写
	UPROPERTY(EditAnywhere, Category="UCS")
	TSoftClassPtr<ULoadScreenUserWidget> LoadScreenUserWidgetClass;
	
};
