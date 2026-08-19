// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
/**
 * 
 */
class FUCSS_TickableSubsystem : public FTickableGameObject
{
public:
	FUCSS_TickableSubsystem()
		: FTickableGameObject(),
		  bIsTickEnabled(false),
		  bIsTickableWhenPaused(false)
	{
	}

private:
	uint8 bIsTickEnabled:1;
	uint8 bIsTickableWhenPaused:1;

public:
	// FTickableObjectBase implementation Begin
	virtual TStatId GetStatId() const override =0;
	virtual bool IsTickableInEditor() const override final { return false; }

	virtual void Tick(float DeltaSeconds) override =0;

	// FTickableObjectBase implementation End

	// FTickableGameObject implementation Begin
	virtual bool IsTickable() const override { return bIsTickEnabled; }
	virtual bool IsTickableWhenPaused() const override { return bIsTickableWhenPaused; }
	// FTickableGameObject implementation End
public:
	virtual void SetTickEnabled(bool const bEnabled) { bIsTickEnabled = bEnabled; }
	virtual void SetTickableWhenPaused(bool const bTickableWhenPaused) { bIsTickableWhenPaused = bTickableWhenPaused; }
};
