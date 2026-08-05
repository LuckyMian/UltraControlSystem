// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UCS_Interface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../Support/UCS_Settings.h"
//#include "UltraControlSystem/Slate/UCS_MenuBase.h"
//#include "UltraControlSystem/Actor/UCS_Widget.h"
#include "UCS_GameInstanceSubsystem.generated.h"

struct FUCS_LogicStruct;
class AUCS_PawnBase;
class AUCS_Camera;
class AUCS_Widget;
class AUCS_PlayerController;
/**
 * 
 */
UCLASS()
class ULTRACONTROLSYSTEM_API UUCS_GameInstanceSubsystem : public UGameInstanceSubsystem, public IUCS_Interface
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	UUCS_GlobalSettings* UCS_GameInstanceSettings;
	
	UPROPERTY()
	AUCS_PawnBase* UCS_Pawn;
	
	UPROPERTY()
	AUCS_PlayerController* UCS_PlayerController;
	
	UPROPERTY(BlueprintReadOnly,Category="UCS")
	TArray<AUCS_Camera*> Cameras;
	
	UPROPERTY(BlueprintReadOnly,Category="UCS")
	TArray<AUCS_Widget*> Widgets;
	
	
	
	// 重写初始化函数
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 重写反初始化函数（可选，用于清理内存、取消绑定事件等）
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category = "UCS")
	bool CallMessanger(FUCS_LogicStruct LogicInput);
	
	UFUNCTION()
	bool FocusCamera(FUCS_LogicStruct LogicInput);
	
	UFUNCTION()
	bool CallInterfaceMessanger(FUCS_LogicStruct LogicInput);
	
	UFUNCTION()
	bool ShowWidgetByMessanger(FUCS_LogicStruct LogicInput);
	
	UFUNCTION()
	bool CallMasterMenuByMessanger(FUCS_LogicStruct LogicInput);
};
