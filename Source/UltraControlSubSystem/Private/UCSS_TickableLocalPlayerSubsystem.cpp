// Copyright YTSS 2023. All Rights Reserved.

#include "UCSS_TickableLocalPlayerSubsystem.h"
#include "UltraControlSubSystem/UCSS_TickableSubsystem.h"

void UUCSS_TickableLocalPlayerSubsystem::Tick(float DeltaSeconds)
{
	checkf(bInitialized,
	       TEXT(
		       "Ticking should have been disabled for an uninitialized subsystem : remember to call IsInitialized in the subsystem's IsTickable, IsTickableInEditor and/or IsTickableWhenPaused implementation"
	       ));
	ReceiveTick(DeltaSeconds);
}

ETickableTickType UUCSS_TickableLocalPlayerSubsystem::GetTickableTickType() const
{
	return IsTemplate() ? ETickableTickType::Never : FUCSS_TickableSubsystem::GetTickableTickType();
}

bool UUCSS_TickableLocalPlayerSubsystem::IsTickable() const
{
	return FUCSS_TickableSubsystem::IsTickable() && !IsTemplate() && bInitialized && IsActive();
}

void UUCSS_TickableLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SetTickEnabled(bIsTickEnabled);
	SetTickableWhenPaused(bIsTickableWhenPaused);
}
