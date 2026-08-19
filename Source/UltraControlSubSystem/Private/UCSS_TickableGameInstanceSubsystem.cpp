// Copyright YTSS 2023. All Rights Reserved.

#include "UCSS_TickableGameInstanceSubsystem.h"
#include "UltraControlSubSystem/UCSS_TickableSubsystem.h"

void UUCSS_TickableGameInstanceSubsystem::Tick(float DeltaTime)
{
	checkf(bInitialized,
	       TEXT(
		       "Ticking should have been disabled for an uninitialized subsystem : remember to call IsInitialized in the subsystem's IsTickable, IsTickableInEditor and/or IsTickableWhenPaused implementation"
	       ));
	ReceiveTick(DeltaTime);
}

ETickableTickType UUCSS_TickableGameInstanceSubsystem::GetTickableTickType() const
{
	return IsTemplate() ? ETickableTickType::Never : FUCSS_TickableSubsystem::GetTickableTickType();
}

bool UUCSS_TickableGameInstanceSubsystem::IsTickable() const
{
	return FUCSS_TickableSubsystem::IsTickable() && !IsTemplate() && bInitialized && IsActive();
}

void UUCSS_TickableGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SetTickEnabled(bIsTickEnabled);
	SetTickableWhenPaused(bIsTickableWhenPaused);
}
