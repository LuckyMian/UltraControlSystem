// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UCS_Interface.h"
#include "WebSocketJsonReceiver.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Containers/Ticker.h"
#include "../Support/UCS_LogicStruct.h"
#include "../Support/UCS_PendingMessageQueue.h"
#include "../Support/UCS_Settings.h"
//#include "UltraControlSystem/Slate/UCS_MenuBase.h"
//#include "UltraControlSystem/Actor/UCS_Widget.h"
#include "UCS_GameInstanceSubsystem.generated.h"

class AUCS_PawnBase;
class AUCS_Camera;
class AUCS_Widget;
class AUCS_PlayerController;
class UUCS_MenuBase;

/**
 * One deferred UCS delivery owed by the message queue.
 *
 * Either a receiver that spawned after the message was sent (matched through its Actor
 * Tags at flush time), or a focus-only request for a camera that already existed while
 * the Pawn was still loading.
 */
struct FUCS_PendingDelivery
{
	/** Receiver waiting for catch-up; null for focus-only requests. */
	TWeakObjectPtr<UObject> Receiver;

	/** State replayed by focus-only requests. */
	FUCS_LogicStruct Logic;

	/** Focus-only requests retry this key; receiver entries leave it empty and match by Tag. */
	FString FocusKey;

	/** Keys already delivered to this receiver, so a retry never repeats a successful channel. */
	TSet<FString> DeliveredKeys;

	/** Retry window; the entry is dropped with a warning once this instant has passed. */
	double DeadlineSeconds = 0.0;

	/** Frame the delivery was created in; flush waits for a later frame so BeginPlay finishes first. */
	uint64 CreatedFrame = 0;

	/** True for the single menu widget, which receives the latest state without Tag matching. */
	bool bRouteToMenu = false;
};

/**
 * 
 */
UCLASS()
class ULTRACONTROLSYSTEM_API UUCS_GameInstanceSubsystem : public UGameInstanceSubsystem, public IUCS_Interface, public IWebSocketJsonReceiver
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

	UPROPERTY(BlueprintReadOnly, Category="UCS")
	FUCS_LogicStruct LogicSave;
	
	
	
	// 重写初始化函数
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 重写反初始化函数（可选，用于清理内存、取消绑定事件等）
	virtual void Deinitialize() override;

	virtual void OnWebSocketJsonReceived_Implementation(const FJsonObjectWrapper& JsonObject) override;
	
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

	/**
	 * Opts a receiver into late catch-up. Scene Actors call this from BeginPlay, and
	 * Blueprint actors/widgets can call it manually.
	 */
	UFUNCTION(BlueprintCallable, Category = "UCS|Message Queue")
	void RegisterUCSReceiver(UObject* Receiver);

	/** Drops every retained state and every pending catch-up delivery. */
	UFUNCTION(BlueprintCallable, Category = "UCS|Message Queue")
	void ClearPendingMessages();

private:
	/** Last-wins store of active UCS states, keyed by LogicMap["Key"]. */
	FUCS_PendingMessageQueue PendingMessages;

	/** Deliveries still owed to receivers that appeared after the message was sent. */
	TArray<FUCS_PendingDelivery> PendingDeliveries;

	FTSTicker::FDelegateHandle FlushTickerHandle;

	void QueuePendingMessage(const FUCS_LogicStruct& LogicInput);
	void ScheduleFocusRetry(const FUCS_LogicStruct& LogicInput, const FString& Key);
	AUCS_PawnBase* ResolveUCSPawn();
	AUCS_Camera* FindCameraByTag(const FString& Key) const;
	void EnsureFlushScheduled();
	void StopFlushTicker();
	bool TickPendingDeliveries(float /*DeltaTime*/);
	bool FlushPendingDeliveries();
	bool DeliverPendingMessage(AActor* Receiver, const FUCS_QueuedLogicMessage& Message);
};
