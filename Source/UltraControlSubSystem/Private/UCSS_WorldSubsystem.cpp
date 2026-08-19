// Copyright YTSS 2023. All Rights Reserved.

#include "UCSS_WorldSubsystem.h"
#include "UltraControlSubSystem/UCSS_SubsystemType.h"

bool UUCSS_WorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && ReceiveShouldCreateSubsystem(Outer);
}

void UUCSS_WorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized)
	
	ReceiveInitialize();
	SetActive(bActiveDefault);
	
	bInitialized=true;
	
	UE_LOG(LogUltraControlSubSystem, Log, TEXT("%s_Initialize"), *GetName())
}

void UUCSS_WorldSubsystem::Deinitialize()
{
	check(bInitialized)
	ReceiveDeinitialize();
	bInitialized=false;
	UE_LOG(LogUltraControlSubSystem, Log, TEXT("%s_Deinitialize"), *GetName())
}

bool UUCSS_WorldSubsystem::ReceiveShouldCreateSubsystem_Implementation(UObject* Outer) const
{
	return true;
}
