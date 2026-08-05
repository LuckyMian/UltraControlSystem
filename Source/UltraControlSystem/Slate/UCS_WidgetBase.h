// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UCS_Interface.h"
#include "../Support/UCS_LogicStruct.h"
#include "UCS_WidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class ULTRACONTROLSYSTEM_API UUCS_WidgetBase : public UUserWidget,public IUCS_Interface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="UCS",Meta = (ExposeOnSpawn = true))
	FUCS_LogicStruct Info;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="UCS")
	bool bCanChili = false;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCS")
	void InitWidget(); 
	virtual void InitWidget_Implementation();
protected:
	
	virtual bool Initialize() override;
};
