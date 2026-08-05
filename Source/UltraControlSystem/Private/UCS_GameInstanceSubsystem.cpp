// Fill out your copyright notice in the Description page of Project Settings.


#include "UCS_GameInstanceSubsystem.h"

#include "UCS_PawnBase.h"
#include "UCS_PlayerController.h"
#include "../Actor/UCS_Camera.h"
#include "Kismet/GameplayStatics.h"
#include "UltraControlSystem/Actor/UCS_Widget.h"
#include "UltraControlSystem/Support/UCS_LogicStruct.h"

void UUCS_GameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UCS_GameInstanceSettings = NewObject<UUCS_GlobalSettings>(this);
}

void UUCS_GameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UUCS_GameInstanceSubsystem::CallMessanger(FUCS_LogicStruct LogicInput)
{
	FocusCamera(LogicInput);
	CallInterfaceMessanger(LogicInput);
	ShowWidgetByMessanger(LogicInput);
	return true;
}

bool UUCS_GameInstanceSubsystem::FocusCamera(FUCS_LogicStruct LogicInput)
{
	
	const FString* KeyPtr = LogicInput.LogicMap.Find(TEXT("Key"));
	if (!KeyPtr) 
	{
		return false; 
	}
	FName TargetTag = FName(**KeyPtr);
	// 2. 遍历相机
	for (AUCS_Camera* Camera : this->Cameras)
	{
		if (Camera)
		{
			if (Camera->ActorHasTag(TargetTag))
			{
				// 3. 注意：直接调用 _Implementation 只能触发 C++ 的实现。
				// 如果该接口可以在蓝图中被重写，建议使用全局静态方法：
				// IFocusPoiInterface::Execute_FocusPoiInterface(UCS_Pawn, Camera);
             
				UCS_Pawn->FocusPoiInterface_Implementation(Camera);
				UUCS_FunctionLibrary::PrintUCSLog(LogicInput,Camera,TEXT("Focus camera"));
				return true;
			}
		}
	}
	return false;
	
}

bool UUCS_GameInstanceSubsystem::CallInterfaceMessanger(FUCS_LogicStruct LogicInput)
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithInterface(this,UUCS_Interface::StaticClass(),OutActors);
	// 如果没有找到任何实现该接口的 Actor，直接返回 false
	if (OutActors.Num() == 0)
	{
		return false;
	}

	// 遍历所有 Actor，每一个都会正确触发（包括蓝图重写的）
	for (AActor* Actor : OutActors)
	{
		if (Actor)
		{
			IUCS_Interface::Execute_LogicInterface(Actor, LogicInput);
		}
	}
    
	// 所有 Actor 都通知完毕后，返回 true
	return true;
}

bool UUCS_GameInstanceSubsystem::ShowWidgetByMessanger(FUCS_LogicStruct LogicInput)
{
	const FString* KeyPtr = LogicInput.LogicMap.Find(TEXT("Key"));
	for (AUCS_Widget* Widget : this->Widgets)
	{
		if (Widget)
		{
			if (Widget->ActorHasTag(FName(*KeyPtr)))
			{
				Widget->ShowSelf(LogicInput);
			}
			
		}
	}
	return true;
}

bool UUCS_GameInstanceSubsystem::CallMasterMenuByMessanger(
	FUCS_LogicStruct LogicInput)
{
	AUCS_PlayerController* PlayerController =
		Cast<AUCS_PlayerController>(
			UGameplayStatics::GetPlayerController(this, 0));

	if (PlayerController == nullptr || PlayerController->UCSWidget == nullptr)
	{
		return false;
	}

	return IUCS_Interface::Execute_LogicInterface(
		PlayerController->UCSWidget,
		LogicInput);
}

