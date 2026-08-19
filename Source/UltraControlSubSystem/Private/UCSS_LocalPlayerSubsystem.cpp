// Copyright YTSS 2023. All Rights Reserved.

#include "UCSS_LocalPlayerSubsystem.h"
#include "UltraControlSubSystem/UCSS_SubsystemType.h"


bool UUCSS_LocalPlayerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && ReceiveShouldCreateSubsystem(Outer);
}

void UUCSS_LocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized)

	// World delegation assign

	FWorldDelegates::OnStartGameInstance.AddUObject(this, &UUCSS_LocalPlayerSubsystem::StartGameInstance);
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UUCSS_LocalPlayerSubsystem::PostWorldInitialization);

	FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &UUCSS_LocalPlayerSubsystem::WorldBeginTearingDown);

	ReceiveInitialize();
	SetActive(bActiveDefault);

	bInitialized = true;

	UE_LOG(LogUltraControlSubSystem, Log, TEXT("%s_Initialize"), *GetName())
}

void UUCSS_LocalPlayerSubsystem::Deinitialize()
{
	check(bInitialized);
	ReceiveDeinitialize();
	bInitialized = false;
	UE_LOG(LogUltraControlSubSystem, Log, TEXT("%s_Deinitialize"), *GetName())
}

void UUCSS_LocalPlayerSubsystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	Super::PlayerControllerChanged(NewPlayerController);
	ReceivePlayerControllerChanged(NewPlayerController);
}

bool UUCSS_LocalPlayerSubsystem::ReceiveShouldCreateSubsystem_Implementation(UObject* Outer) const
{
	return true;
}
