// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UltraControlSubSystem/UCSS_TickableSubsystem.h"
#include "UCSS_LocalPlayerSubsystem.h"
#include "UCSS_TickableLocalPlayerSubsystem.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType)
class ULTRACONTROLSUBSYSTEM_API UUCSS_TickableLocalPlayerSubsystem : public UUCSS_LocalPlayerSubsystem,
                                                                        public FUCSS_TickableSubsystem
{
	GENERATED_BODY()

public:
	UUCSS_TickableLocalPlayerSubsystem()
		: UUCSS_LocalPlayerSubsystem(),
		  bIsTickEnabled(true),
		  bIsTickableWhenPaused(false)
	{
	}

	// USubsystem implementation Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// USubsystem implementation End

private:
	//Defaults Begin

	/**Whether to enable Tick*/
	UPROPERTY(EditAnywhere, Category="Tick")
	uint8 bIsTickEnabled:1;

	/**Whether to enable Tick when paused*/
	UPROPERTY(EditAnywhere, Category="Tick")
	uint8 bIsTickableWhenPaused:1;

	//Defaults End

public:
	//FUCSS_TickableSubsystem implementation Begin
	virtual void Tick(float DeltaSeconds) override;
	virtual TStatId GetStatId() const override final { return Super::GetStatID(); }
	virtual UWorld* GetTickableGameObjectWorld() const override final { return GetWorld(); }
	virtual ETickableTickType GetTickableTickType() const override final;
	//FUCSS_TickableSubsystem implementation End


protected:
	//Blueprint function Begin

	/**Event called every frame*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Tick")
	void ReceiveTick(float DeltaSeconds);

	/** 
	 * Set this Subsystem's tick functions to be enabled or disabled.
	 * This only modifies the tick function on this Subsystem
	 * @param bEnabled - Whether it should be enabled or not
	 */
	UFUNCTION(BlueprintCallable, Category="Subsystem|Tick")
	virtual void SetTickEnabled(bool const bEnabled) override final { FUCSS_TickableSubsystem::SetTickEnabled(bEnabled); }

	/**
	 * Returns whether this Subsystem has tick enabled or not
	 */
	UFUNCTION(BlueprintCallable, Category="Subsystem|Tick", meta=(KeyWords="Get"))
	virtual bool IsTickable() const override final;

	/**
	 * Sets whether this actor can tick when paused.
	 * @param bTickableWhenPaused - Whether it should be Tickable or not when paused
	 */
	UFUNCTION(BlueprintCallable, Category="Subsystem|Tick")
	virtual void SetTickableWhenPaused(bool const bTickableWhenPaused) override final
	{
		FUCSS_TickableSubsystem::SetTickableWhenPaused(bTickableWhenPaused);
	}

	/**
	 * Returns whether this Subsystem has tick enabled or not when paused
	 */
	UFUNCTION(BlueprintCallable, Category="Subsystem|Tick", meta=(KeyWords="Get"))
	virtual bool IsTickableWhenPaused() const override final { return FUCSS_TickableSubsystem::IsTickableWhenPaused(); }

	//Blueprint function End
};
