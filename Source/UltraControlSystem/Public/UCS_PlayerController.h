// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UltraControlSystem/Slate/UCS_MenuBase.h>

#include "CoreMinimal.h"
#include "UCS_Interface.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "UCS_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UUCS_WidgetBase;
class SBlackScreen;

/**
 *
 */
UCLASS()
class ULTRACONTROLSYSTEM_API AUCS_PlayerController : public APlayerController, public IUCS_Interface
{
	GENERATED_BODY()
	

public:
	AUCS_PlayerController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	UFUNCTION()
	void PlayBlackScreenTimeline();
	
	UPROPERTY()
	TObjectPtr<UUCS_MenuBase> UCSWidget;
	
	UPROPERTY()
	UUCS_GameInstanceSubsystem* GameInstance; //子系统变量

private:
	void OnMove(const FInputActionValue& Value);
	void OnRotation(const FInputActionValue& Value);
	void OnZoom(const FInputActionValue& Value);

protected:
	// 在编辑器中指定蓝图资产
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> RotationAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ZoomAction;


	
	
	TSharedPtr<SBlackScreen> BlackScreenWidget;
	

	
};
