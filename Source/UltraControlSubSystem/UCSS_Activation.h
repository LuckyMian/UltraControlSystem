// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FUCSS_Activation
{
public:
	FUCSS_Activation():bIsActive(false){}
	FUCSS_Activation(bool bAutoActive):bIsActive(bAutoActive){}
	virtual ~FUCSS_Activation(){};

private:
	uint8 bIsActive:1;

public:
	/**
	 * Activates the object.
	 * @param bReset - Whether the activation should happen even if IsActive returns true.
	 */
	virtual void Activate(bool bReset = false)
	{
		if (bReset || !IsActive())
		{
			bIsActive = true;
			ReceiveActivate(bReset);
		}
	}

	/**
	 * Deactivates the object.
	 */
	virtual void Deactivate()
	{
		if (IsActive())
		{
			bIsActive = false;
			ReceiveDeactivate();
		}
	}

	/**
	 * Sets whether the object is active or not
	 * @param bNewActive - The new active state of the object
	 * @param bReset - Whether the activation should happen even if IsActive returns true.
	 */
	virtual void SetActive(bool bNewActive, bool bReset = false) { bNewActive ? Activate(bReset) : Deactivate(); }

	/**
	 * Toggles the active state of the object
	 */
	virtual void ToggleActive() { SetActive(!IsActive()); }

	/**
	 * Returns whether the object is active or not
	 * @return - The active state of the object.
	 */
	FORCEINLINE bool IsActive() const { return bIsActive; }

protected:
	/**
	 * Called when effectively activated
	 * should be overridden by native child classes.
	 */
	virtual void ReceiveActivate(bool bReset = false) {};
	/**
	 * Called when effectively deactivated
	 * should be overridden by native child classes.
	 */
	virtual void ReceiveDeactivate() {};
};
