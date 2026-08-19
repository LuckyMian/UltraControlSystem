// Copyright YTSS 2023. All Rights Reserved.

#include "UCSS_GameInstanceSubsystem.h"
#include "UltraControlSubSystem/UCSS_SubsystemType.h"





bool UUCSS_GameInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && ReceiveShouldCreateSubsystem(Outer);
}

void UUCSS_GameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized)
	
	// World delegation assign
	
	// FWorldDelegates::OnStartGameInstance.AddUObject(this, &UUCSS_GameInstanceSubsystem::StartGameInstance);
	FWorldDelegates::OnPostWorldInitialization.
		AddUObject(this, &UUCSS_GameInstanceSubsystem::PostWorldInitialization);
	FWorldDelegates::OnWorldBeginTearDown.AddUObject(this,&UUCSS_GameInstanceSubsystem::WorldBeginTearingDown);

	ReceiveInitialize();
	SetActive(bActiveDefault);

	bInitialized = true;

	UE_LOG(LogUltraControlSubSystem, Log, TEXT("%s_Initialize"), *GetName())
}

void UUCSS_GameInstanceSubsystem::Deinitialize()
{
	check(bInitialized)
	ReceiveDeinitialize();
	bInitialized = false;
	UE_LOG(LogUltraControlSubSystem, Log, TEXT("%s_Deinitialize"), *GetName())
}

bool UUCSS_GameInstanceSubsystem::ReceiveShouldCreateSubsystem_Implementation(UObject* Outer) const
{
	return true;
}
