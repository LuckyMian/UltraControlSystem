// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UCS_Interface.h"
#include "UCS_MenuBase.generated.h"

class UWidgetAnimation;

/**
 * Base class for UltraControlSystem menu widgets.
 */
UCLASS()
class ULTRACONTROLSYSTEM_API UUCS_MenuBase : public UUserWidget, public IUCS_Interface
{
	GENERATED_BODY()

public:
	virtual bool ShowPage(FUCS_LogicStruct LogicInput) override;

	/** Plays the animation named PageAnimation supplied by the concrete Widget Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "UCS|Animation")
	bool PlayPageAnimation();

	/** Legacy fallback for widgets that have not yet added a bound PageAnimation. */
	UFUNCTION(BlueprintImplementableEvent, Category = "UCS")
	void StartPageAnimation();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UCS", Meta = (ExposeOnSpawn = true))
	FUCS_LogicStruct Logic;

protected:
	/**
	 * Each concrete Widget Blueprint can create an animation named PageAnimation
	 * and author completely different tracks and keyframes.
	 */
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> PageAnimation;
};
