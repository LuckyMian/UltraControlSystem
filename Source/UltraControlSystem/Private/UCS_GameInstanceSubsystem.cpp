// Fill out your copyright notice in the Description page of Project Settings.


#include "UCS_GameInstanceSubsystem.h"

#include "UCS_FunctionLibrary.h"
#include "UCS_PawnBase.h"
#include "UCS_PlayerController.h"
#include "../Actor/UCS_Camera.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
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
	const FString* RetriggerValue = LogicInput.LogicMap.Find(TEXT("Retrigger"));
	const bool bPreventRetrigger = RetriggerValue == nullptr || *RetriggerValue == TEXT("0");
	if (bPreventRetrigger)
	{
		const FString* SavedKey = LogicSave.LogicMap.Find(TEXT("Key"));
		const FString* InputKey = LogicInput.LogicMap.Find(TEXT("Key"));
		if (SavedKey != nullptr && InputKey != nullptr
			&& !SavedKey->IsEmpty() && !InputKey->IsEmpty()
			&& UUCS_FunctionLibrary::MatchesCommaSeparatedString(*SavedKey, *InputKey))
		{
			return false;
		}
	}

	LogicSave = LogicInput;
	FocusCamera(LogicInput);
	CallInterfaceMessanger(LogicInput);
	ShowWidgetByMessanger(LogicInput);
	return true;
}

bool UUCS_GameInstanceSubsystem::FocusCamera(FUCS_LogicStruct LogicInput)
{
	const FString* KeyPtr = LogicInput.LogicMap.Find(TEXT("Key"));
	if (KeyPtr == nullptr || KeyPtr->IsEmpty())
	{
		return false;
	}

	// Some maps use the engine's default GameMode and therefore do not spawn an
	// AUCS_PawnBase. Resolve it lazily when possible and never dereference null.
	if (!IsValid(UCS_Pawn))
	{
		UCS_Pawn = Cast<AUCS_PawnBase>(UGameplayStatics::GetPlayerPawn(this, 0));
	}
	if (!IsValid(UCS_Pawn))
	{
		return false;
	}

	FName TargetTag = FName(**KeyPtr);
	// 2. 遍历相机
	for (AUCS_Camera* Camera : this->Cameras)
	{
		if (IsValid(Camera))
		{
			if (Camera->ActorHasTag(TargetTag))
			{
				IUCS_Interface::Execute_FocusPoiInterface(UCS_Pawn, Camera);
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
	if (KeyPtr == nullptr || KeyPtr->IsEmpty())
	{
		return false;
	}

	for (AUCS_Widget* Widget : this->Widgets)
	{
		if (IsValid(Widget))
		{
			if (Widget->ActorHasTag(FName(*KeyPtr)))
			{
				Widget->ShowSelf(LogicInput);
			}
			
		}
	}
	return true;
}

void UUCS_GameInstanceSubsystem::OnWebSocketJsonReceived_Implementation(const FJsonObjectWrapper& JsonObject)
{
	if (!JsonObject.JsonObject.IsValid())
	{
		return;
	}

	FString Command;
	if (!JsonObject.JsonObject->TryGetStringField(TEXT("command"), Command)
		|| !Command.Equals(TEXT("camera_info"), ESearchCase::IgnoreCase))
	{
		return;
	}

	const TSharedPtr<FJsonObject>* InfoObject = nullptr;
	if (!JsonObject.JsonObject->TryGetObjectField(TEXT("Info"), InfoObject)
		|| InfoObject == nullptr
		|| !InfoObject->IsValid())
	{
		return;
	}

	FUCS_LogicStruct LogicInput;
	if (!FJsonObjectConverter::JsonObjectToUStruct(
		InfoObject->ToSharedRef(),
		FUCS_LogicStruct::StaticStruct(),
		&LogicInput))
	{
		return;
	}

	const FString* Key = LogicInput.LogicMap.Find(TEXT("Key"));
	if (Key == nullptr || Key->IsEmpty())
	{
		return;
	}

	CallMessanger(MoveTemp(LogicInput));
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

