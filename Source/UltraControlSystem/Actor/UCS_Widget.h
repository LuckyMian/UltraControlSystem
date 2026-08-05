// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UCS_ActorBase.h"
#include "Components/WidgetComponent.h"
#include "UCS_Widget.generated.h"

class UUCS_WidgetBase;

UCLASS()
class ULTRACONTROLSYSTEM_API AUCS_Widget : public AUCS_ActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUCS_Widget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
private:


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// 1. 声明 Widget 组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UCS")
	UWidgetComponent* Widget;

	// 2. 允许在蓝图/面板中指定具体的 Widget 蓝图资产
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UCS")
	TSubclassOf<UUCS_WidgetBase> WidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UCS")
	TArray<FName> Groups;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="UCS",Meta = (ExposeOnSpawn = true))
	FUCS_LogicStruct Info;
	
	UFUNCTION(BlueprintCallable, Category = "UMG|ScreenSpaceTransform")
	void SetScreenSpaceWidgetScaleAndPivot(UWidgetComponent* TargetComponent, FVector2D NewScale2D, FVector2D NewPivot2D);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCS")
	bool ShowSelf(FUCS_LogicStruct LogicInput);
	virtual bool ShowSelf_Implementation(FUCS_LogicStruct LogicInput);
};
