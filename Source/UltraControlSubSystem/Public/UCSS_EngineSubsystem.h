// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "UCSS_EngineSubsystem.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ULTRACONTROLSUBSYSTEM_API UUCSS_EngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	// USubsystem implementation Begin
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// USubsystem implementation End
protected:
	/**The event called after this subsystem is created.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Initialize")
	void ReceiveInitialize();

	/**The event called before this subsystem is destroyed.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Deinitialize")
	void ReceiveDeinitialize();

	/**
	 * The event called before this subsystem is created.
	 * This event is used to determine whether to create the subsystem singleton.
	 * @param Outer - The outer of UCSS_EditorSubsystem should be a UEngine
	 * @return - The return value determines whether to create the Subsystem
	 */
	UFUNCTION(BlueprintNativeEvent, DisplayName="ShouldCreateSubsystem")
	bool ReceiveShouldCreateSubsystem(UObject* Outer) const;
};
