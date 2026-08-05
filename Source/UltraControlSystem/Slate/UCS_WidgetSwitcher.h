// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetSwitcher.h"
#include "../Support/UCS_LogicStruct.h"
#include "UCS_WidgetSwitcher.generated.h"

class UUCS_MenuBase;

/**
 * A widget switcher that selects UUCS_MenuBase children by LogicMap["Key"].
 */
UCLASS(meta = (DisplayName = "UCS Widget Switcher"))
class ULTRACONTROLSYSTEM_API UUCS_WidgetSwitcher : public UWidgetSwitcher
{
	GENERATED_BODY()

public:
	/** Activates the first menu whose LogicMap["Key"] exactly matches Key. */
	UFUNCTION(BlueprintCallable, Category = "UCS|Panel")
	bool SetActiveWidgetByKey(const FString& Key);

	/** Reads LogicInput.LogicMap["Key"] and activates the matching menu. */
	UFUNCTION(BlueprintCallable, Category = "UCS|Panel")
	bool SetActiveWidgetByLogic(const FUCS_LogicStruct& LogicInput);

	/** Returns the first menu whose LogicMap["Key"] exactly matches Key. */
	UFUNCTION(BlueprintPure, Category = "UCS|Panel")
	UUCS_MenuBase* GetWidgetByKey(const FString& Key) const;

	/** Returns LogicMap["Key"] from the active menu, or an empty string. */
	UFUNCTION(BlueprintPure, Category = "UCS|Panel")
	FString GetActiveWidgetKey() const;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

private:
	int32 FindWidgetIndexByKey(const FString& Key) const;
};
