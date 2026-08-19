// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UltraControlSubSystem/UCSS_Activation.h"
#include "Engine/World.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "UCSS_LocalPlayerSubsystem.generated.h"

/**
 * UUCSS_LocalPlayerSubsystem
 * 提供可以派生出子蓝图类的本地玩家子系统基�? * Provides a LocalPlayerSubsystem base class from which SubBlueprint classes can be derived
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class ULTRACONTROLSUBSYSTEM_API UUCSS_LocalPlayerSubsystem : public ULocalPlayerSubsystem, public FUCSS_Activation
{
	GENERATED_BODY()

public:
	UUCSS_LocalPlayerSubsystem()
		: FUCSS_Activation(true),
		  bInitialized(false),
		  bActiveDefault(true)
	{
	}

	virtual UWorld* GetWorld() const override final
	{
		return IsValid(Cast<UObject>(GetLocalPlayerController())) ? GetLocalPlayerController()->GetWorld() : nullptr;
	}

protected:
	uint8 bInitialized : 1;

private:
	// Middle function Begin
	FORCEINLINE void PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);
	FORCEINLINE void StartGameInstance(UGameInstance* GameInstance);
	// Middle function End
public:
	// ULocalPlayerSubsystem implementation Begin
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void PlayerControllerChanged(APlayerController* NewPlayerController) override;
	// ULocalPlayerSubsystem implementation End

	virtual void WorldBeginPlay() { ReceiveWorldBeginPlay(); }
	virtual void WorldBeginTearingDown(UWorld* World) { ReceiveWorldBeginTearingDown(); }

protected:
	//Blueprint functions Begin

	/**The event called after this subsystem is created.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Initialize")
	void ReceiveInitialize();

	/**The event called before this subsystem is destroyed.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Deinitialize")
	void ReceiveDeinitialize();

	/**The event called after world begin play.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="WorldBeginPlay")
	void ReceiveWorldBeginPlay();

	/**The event called before world tear down.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="WorldBeginTearDown")
	void ReceiveWorldBeginTearingDown();

	/** Callback for when the player controller is changed on this subsystem's owning local player */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="PlayerControllerChanged")
	void ReceivePlayerControllerChanged(APlayerController* NewPlayerController);

	/**
	 * This event is called before the subsystem is constructed.
	 * Used to determine if the subsystem is created.
	 * Attention! This subsystem had not been constructed at the time this event was executed,
	 * and any access to the subsystem member variables in this event occurs only on the CDO object,
	 * not on the actual generated subsystem object.
	 * You should use this function as a global static function, not as a member function.
	 * @param Outer - The outer of UUCSS_LocalPlayerSubsystem should be a ULocalPlayer
	 * @return - The return value determines whether to create the Subsystem
	 */
	UFUNCTION(BlueprintNativeEvent, DisplayName="ShouldCreateSubsystem")
	bool ReceiveShouldCreateSubsystem(UObject* Outer) const;

public:
	/**
	 * Return PlayerController of this Subsystem
	 * PlayerController does not exist when Initialize is executing
	 */
	UFUNCTION(BlueprintPure, Category="Subsystem|LocalPlayerSubsystem")
	FORCEINLINE APlayerController* GetLocalPlayerController() const
	{
		return GetLocalPlayer() ? GetLocalPlayer()->PlayerController : nullptr;
	}

	//Blueprint functions End

public:
	// FUCSS_Activation implementation Begin

	/**
	 * Activates the object.
	 * @param bReset - Whether the activation should happen even if IsActive returns true.
	 */
	UFUNCTION(BlueprintCallable, Category="Activation")
	virtual void Activate(bool bReset = false) override { FUCSS_Activation::Activate(bReset); }

	/**
	 * Deactivates the object.
	 */
	UFUNCTION(BlueprintCallable, Category="Activation")
	virtual void Deactivate() override { FUCSS_Activation::Deactivate(); };

	/**
	 * Sets whether the object is active or not
	 * @param bNewActive - The new active state of the object
	 * @param bReset - Whether the activation should happen even if IsActive returns true.
	 */
	UFUNCTION(BlueprintCallable, Category="Activation")
	virtual void SetActive(bool bNewActive, bool bReset = false) override final
	{
		FUCSS_Activation::SetActive(bNewActive, bReset);
	}

	/**
	 * Toggles the active state of the object
	 */
	UFUNCTION(BlueprintCallable, Category="Activation")
	virtual void ToggleActive() override final { FUCSS_Activation::ToggleActive(); }

	/**
	 * Returns whether the object is active or not
	 * @return - The active state of the object.
	 */
	UFUNCTION(BlueprintCallable, Category="Activation", meta=(KeyWords="Get"))
	FORCEINLINE bool IsActive() const { return FUCSS_Activation::IsActive(); }

	DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FActivatedSignature, UUCSS_LocalPlayerSubsystem, OnActivated,
	                                                    UObject*, Object, bool, bReset);

	DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FDeactivateSignature, UUCSS_LocalPlayerSubsystem,
	                                                   OnDeactivated, UObject*, Object);

	/** Called when the component has been activated, with parameter indicating if it was from a reset */
	UPROPERTY(BlueprintAssignable, Category="Activation")
	FActivatedSignature OnActivated;
	/** Called when the component has been deactivated */
	UPROPERTY(BlueprintAssignable, Category="Activation")
	FDeactivateSignature OnDeactivated;

protected:
	/** Default activation status */
	UPROPERTY(EditAnywhere, Category="Activation")
	uint8 bActiveDefault : 1;

	virtual void ReceiveActivate(bool bReset) override { if (bInitialized) OnActivated.Broadcast(this, bReset); }
	virtual void ReceiveDeactivate() override { if (bInitialized) OnDeactivated.Broadcast(this); }

	// FUCSS_Activation implementation End
};

void UUCSS_LocalPlayerSubsystem::PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
	World->OnWorldBeginPlay.AddUObject(this, &UUCSS_LocalPlayerSubsystem::WorldBeginPlay);
}

void UUCSS_LocalPlayerSubsystem::StartGameInstance(UGameInstance* GameInstance)
{
	if (GWorld)
	{
		GWorld->OnWorldBeginPlay.AddUObject(this, &UUCSS_LocalPlayerSubsystem::WorldBeginPlay);
	}
}
