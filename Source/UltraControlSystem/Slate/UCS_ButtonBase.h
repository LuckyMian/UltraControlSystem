// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "UCS_Interface.h"
#include "UCS_ButtonBase.generated.h"

/**
 * Base button widget for UltraControlSystem.
 */
UCLASS()
class ULTRACONTROLSYSTEM_API UUCS_ButtonBase : public UButton, public IUCS_Interface
{
	GENERATED_BODY()

public:
	UUCS_ButtonBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UCS", Meta = (ExposeOnSpawn = true))
	FUCS_LogicStruct Logic;
	
#if WITH_EDITOR
	/** Invoked from the Details panel while editing this widget. */
	UFUNCTION(CallInEditor, Category = "UCS")
	void SetName();
#endif

protected:
	/** Called after the underlying Slate widget has been created/rebuilt. */
	virtual void OnWidgetRebuilt() override;

	UFUNCTION()
	void HandleOnPressed();
};
