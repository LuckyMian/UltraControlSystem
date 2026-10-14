// Fill out your copyright notice in the Description page of Project Settings.


#include "UCS_GameInstanceSubsystem.h"

#include "UCS_FunctionLibrary.h"
#include "UCS_PawnBase.h"
#include "UCS_PlayerController.h"
#include "UCS_ProjectSettings.h"
#include "../Actor/UCS_Camera.h"
#include "../Slate/UCS_MenuBase.h"
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

	StopFlushTicker();
	PendingDeliveries.Reset();
	PendingMessages.Clear();
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

	// Keep the state so receivers that spawn after this call can still catch up.
	QueuePendingMessage(LogicInput);
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
	AUCS_PawnBase* Pawn = ResolveUCSPawn();
	if (Pawn == nullptr)
	{
		return false;
	}

	AUCS_Camera* Camera = FindCameraByTag(*KeyPtr);
	if (Camera == nullptr)
	{
		return false;
	}

	IUCS_Interface::Execute_FocusPoiInterface(Pawn, Camera);
	UUCS_FunctionLibrary::PrintUCSLog(LogicInput, Camera, TEXT("Focus camera"));
	return true;
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

// ---------------------------------------------------------------------------
// Message queue: late receiver catch-up
// ---------------------------------------------------------------------------

AUCS_PawnBase* UUCS_GameInstanceSubsystem::ResolveUCSPawn()
{
	if (!IsValid(UCS_Pawn))
	{
		UCS_Pawn = Cast<AUCS_PawnBase>(UGameplayStatics::GetPlayerPawn(this, 0));
	}

	return IsValid(UCS_Pawn) ? UCS_Pawn : nullptr;
}

AUCS_Camera* UUCS_GameInstanceSubsystem::FindCameraByTag(const FString& Key) const
{
	if (Key.IsEmpty())
	{
		return nullptr;
	}

	// 2. 遍历相机
	const FName TargetTag(*Key);
	for (AUCS_Camera* Camera : this->Cameras)
	{
		if (IsValid(Camera) && Camera->ActorHasTag(TargetTag))
		{
			return Camera;
		}
	}
	return nullptr;
}

void UUCS_GameInstanceSubsystem::QueuePendingMessage(const FUCS_LogicStruct& LogicInput)
{
	if (!UUCS_ProjectSettings::IsMessageQueueEnabled())
	{
		return;
	}

	const FString* KeyPtr = LogicInput.LogicMap.Find(TEXT("Key"));
	if (KeyPtr == nullptr || KeyPtr->IsEmpty())
	{
		return;
	}

	PendingMessages.Enqueue(
		LogicInput,
		FPlatformTime::Seconds(),
		UUCS_ProjectSettings::GetMaxPendingMessages());

	// FocusCamera is the only live channel that needs an already spawned Pawn. A camera
	// can already be in the level while the Pawn is still loading, so keep retrying the
	// focus for the configured window instead of losing the state.
	if (FindCameraByTag(*KeyPtr) != nullptr && ResolveUCSPawn() == nullptr)
	{
		ScheduleFocusRetry(LogicInput, *KeyPtr);
	}
}

void UUCS_GameInstanceSubsystem::ScheduleFocusRetry(const FUCS_LogicStruct& LogicInput, const FString& Key)
{
	for (const FUCS_PendingDelivery& Pending : PendingDeliveries)
	{
		if (!Pending.Receiver.IsValid() && Pending.FocusKey.Equals(Key))
		{
			return;
		}
	}

	FUCS_PendingDelivery Delivery;
	Delivery.Logic = LogicInput;
	Delivery.FocusKey = Key;
	Delivery.CreatedFrame = GFrameCounter;
	Delivery.DeadlineSeconds = FPlatformTime::Seconds() + UUCS_ProjectSettings::GetMessageQueueRetrySeconds();
	PendingDeliveries.Add(MoveTemp(Delivery));

	EnsureFlushScheduled();
}

void UUCS_GameInstanceSubsystem::RegisterUCSReceiver(UObject* Receiver)
{
	if (Receiver == nullptr || !UUCS_ProjectSettings::IsMessageQueueEnabled() || PendingMessages.IsEmpty())
	{
		return;
	}

	for (const FUCS_PendingDelivery& Pending : PendingDeliveries)
	{
		if (Pending.Receiver.Get() == Receiver)
		{
			return;
		}
	}

	FUCS_PendingDelivery Delivery;
	Delivery.Receiver = Receiver;
	Delivery.CreatedFrame = GFrameCounter;
	Delivery.DeadlineSeconds = FPlatformTime::Seconds() + UUCS_ProjectSettings::GetMessageQueueRetrySeconds();
	Delivery.bRouteToMenu = Receiver->IsA(UUCS_MenuBase::StaticClass());
	PendingDeliveries.Add(MoveTemp(Delivery));

	EnsureFlushScheduled();
}

void UUCS_GameInstanceSubsystem::ClearPendingMessages()
{
	StopFlushTicker();
	PendingDeliveries.Reset();
	PendingMessages.Clear();
}

void UUCS_GameInstanceSubsystem::EnsureFlushScheduled()
{
	if (FlushTickerHandle.IsValid())
	{
		return;
	}

	FlushTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &UUCS_GameInstanceSubsystem::TickPendingDeliveries));
}

void UUCS_GameInstanceSubsystem::StopFlushTicker()
{
	if (FlushTickerHandle.IsValid())
	{
		FTSTicker::RemoveTicker(FlushTickerHandle);
		FlushTickerHandle.Reset();
	}
}

bool UUCS_GameInstanceSubsystem::TickPendingDeliveries(float /*DeltaTime*/)
{
	if (FlushPendingDeliveries())
	{
		return true;
	}

	// The core ticker drops this entry because we return false; only clear our own
	// handle here so we never remove an entry while it is being ticked.
	FlushTickerHandle.Reset();
	return false;
}

bool UUCS_GameInstanceSubsystem::FlushPendingDeliveries()
{
	if (PendingDeliveries.Num() == 0)
	{
		return false;
	}

	const double NowSeconds = FPlatformTime::Seconds();
	TArray<FUCS_PendingDelivery> Survivors;
	Survivors.Reserve(PendingDeliveries.Num());

	for (FUCS_PendingDelivery& Delivery : PendingDeliveries)
	{
		// Wait for a later frame so every BeginPlay in the spawn frame has finished and the
		// receivers have their Actor Tags plus their entry in Cameras/Widgets.
		if (Delivery.CreatedFrame >= GFrameCounter)
		{
			Survivors.Add(Delivery);
			continue;
		}

		UObject* Receiver = Delivery.Receiver.Get();
		if (Receiver == nullptr && Delivery.FocusKey.IsEmpty())
		{
			// The receiver was destroyed before it could catch up.
			continue;
		}

		bool bNeedsRetry = false;

		if (Receiver == nullptr)
		{
			// Focus-only request: the camera existed, the Pawn did not.
			bNeedsRetry = !FocusCamera(Delivery.Logic);
		}
		else if (Delivery.bRouteToMenu)
		{
			if (const FUCS_QueuedLogicMessage* Latest = PendingMessages.GetLatest())
			{
				IUCS_Interface::Execute_LogicInterface(Receiver, Latest->Logic);
				Delivery.DeliveredKeys.Add(Latest->Key);
			}
		}
		else if (AActor* ReceiverActor = Cast<AActor>(Receiver))
		{
			const TArray<FUCS_QueuedLogicMessage> Matches =
				PendingMessages.FindMatchingMessages(ReceiverActor->Tags);

			for (const FUCS_QueuedLogicMessage& Message : Matches)
			{
				if (Delivery.DeliveredKeys.Contains(Message.Key))
				{
					continue;
				}

				if (DeliverPendingMessage(ReceiverActor, Message))
				{
					Delivery.DeliveredKeys.Add(Message.Key);
				}
				else
				{
					bNeedsRetry = true;
				}
			}
		}

		if (!bNeedsRetry)
		{
			continue;
		}

		if (NowSeconds < Delivery.DeadlineSeconds)
		{
			Survivors.Add(Delivery);
		}
		else
		{
			UE_LOG(LogTemp, Warning,
				TEXT("UCS: dropped a queued message for %s after %.2fs because the UCS Pawn never became available."),
				*GetNameSafe(Receiver),
				UUCS_ProjectSettings::GetMessageQueueRetrySeconds());
		}
	}

	PendingDeliveries = MoveTemp(Survivors);
	return PendingDeliveries.Num() > 0;
}

bool UUCS_GameInstanceSubsystem::DeliverPendingMessage(AActor* Receiver, const FUCS_QueuedLogicMessage& Message)
{
	if (!IsValid(Receiver))
	{
		return true;
	}

	// Tag matching mirrors the live path (AActor::ActorHasTag) so a replayed receiver
	// sees exactly the channels it would have seen in real time.
	if (!Receiver->ActorHasTag(FName(*Message.Key)))
	{
		return true;
	}

	if (Receiver->IsA(AUCS_Camera::StaticClass()))
	{
		// Retry until the Pawn exists inside the configured window, so the broadcast
		// below is not repeated on every retry.
		if (!FocusCamera(Message.Logic))
		{
			return false;
		}
	}
	else if (AUCS_Widget* Widget = Cast<AUCS_Widget>(Receiver))
	{
		Widget->ShowSelf(Message.Logic);
	}

	if (Receiver->GetClass()->ImplementsInterface(UUCS_Interface::StaticClass()))
	{
		IUCS_Interface::Execute_LogicInterface(Receiver, Message.Logic);
	}

	return true;
}

