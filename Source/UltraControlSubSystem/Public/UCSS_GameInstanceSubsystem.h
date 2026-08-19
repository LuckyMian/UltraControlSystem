// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UltraControlSubSystem/UCSS_Activation.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UCSS_GameInstanceSubsystem.generated.h"

// class FUCSS_Activation;
// class UGameInstance;

/**
 * UUCSS_GameInstanceSubsystem
 * 提供可以派生出子蓝图类的游戏实例子系统基�? * Provides a GameInstanceSubsystem base class from which SubBlueprint classes can be derived
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class ULTRACONTROLSUBSYSTEM_API UUCSS_GameInstanceSubsystem : public UGameInstanceSubsystem, public FUCSS_Activation
{
	GENERATED_BODY()

public:
	UUCSS_GameInstanceSubsystem()
		: FUCSS_Activation(true),
		  bInitialized(false),
		  bActiveDefault(true)
	{
	}

	virtual UWorld* GetWorld() const override final
	{
		return GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	}

protected:
	uint8 bInitialized:1;

private:
	// Middle function Begin
	FORCEINLINE void PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);
	FORCEINLINE void StartGameInstance(UGameInstance* GameInstance);
	// Middle function End
public:
	// UGameInstanceSubsystem implementation Begin
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// UGameInstanceSubsystem implementation End

	virtual void WorldBeginPlay() { ReceiveWorldBeginPlay(); }
	virtual void WorldBeginTearingDown(UWorld* World) { ReceiveWorldBeginTearingDown(); }

protected:
	// Blueprint functions Begin

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

	/**
	 * This event is called before the subsystem is constructed.
	 * Used to determine if the subsystem is created.
	 * Attention! This subsystem had not been constructed at the time this event was executed,
	 * and any access to the subsystem member variables in this event occurs only on the CDO object,
	 * not on the actual generated subsystem object.
	 * You should use this function as a global static function, not as a member function.
	 * @param Outer - The outer of UUCSS_LocalPlayerSubsystem should be a UGameInstance
	 * @return - The return value determines whether to create the Subsystem
	 */
	UFUNCTION(BlueprintNativeEvent, DisplayName="ShouldCreateSubsystem")
	bool ReceiveShouldCreateSubsystem(UObject* Outer) const;

public:
	/**Return Outer of this Subsystem*/
	UFUNCTION(BlueprintPure, Category="Subsystem|GameInstanceSubsystem", meta=(KeyWords="Get"))
	FORCEINLINE UGameInstance* GetGameInstance() const { return Super::GetGameInstance(); }

	// Blueprint functions End

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

	DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FActivatedSignature, UUCSS_GameInstanceSubsystem, OnActivated,
	                                                    UObject*, Object, bool, bReset);

	DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FDeactivateSignature, UUCSS_GameInstanceSubsystem,
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
	uint8 bActiveDefault:1;

	virtual void ReceiveActivate(bool bReset) override { if (bInitialized) OnActivated.Broadcast(this, bReset); }
	virtual void ReceiveDeactivate() override { if (bInitialized) OnDeactivated.Broadcast(this); }

	// FUCSS_Activation implementation End
};


void UUCSS_GameInstanceSubsystem::PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
	World->OnWorldBeginPlay.AddUObject(this, &UUCSS_GameInstanceSubsystem::WorldBeginPlay);
}

void UUCSS_GameInstanceSubsystem::StartGameInstance(UGameInstance* GameInstance)
{
	if (GWorld)
	{
		GWorld->OnWorldBeginPlay.AddUObject(this, &UUCSS_GameInstanceSubsystem::WorldBeginPlay);
	}
}