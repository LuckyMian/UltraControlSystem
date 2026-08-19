// Copyright YTSS 2023. All Rights Reserved.

#include "UCSS_EngineSubsystem.h"
#include "UltraControlSubSystem/UCSS_SubsystemType.h"

bool UUCSS_EngineSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && ReceiveShouldCreateSubsystem(Outer);
}

void UUCSS_EngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ReceiveInitialize();
	UE_LOG(LogUltraControlSubSystem, Warning, TEXT("%s_Initialize"), *GetName())
}

void UUCSS_EngineSubsystem::Deinitialize()
{
	ReceiveDeinitialize();
	Super::Deinitialize();
	UE_LOG(LogUltraControlSubSystem, Warning, TEXT("%s_Deinitialize"), *GetName())
}

bool UUCSS_EngineSubsystem::ReceiveShouldCreateSubsystem_Implementation(UObject* Outer) const
{
	return true;
}
