// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "UCSS_PluginProxy.generated.h"

/**
 * 
 */
UCLASS()
class ULTRACONTROLSUBSYSTEM_API UUCSS_PluginProxy : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
