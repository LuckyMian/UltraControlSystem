// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UCS_ActorBase.h"
#include "UCS_GameInstanceSubsystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UCS_FunctionLibrary.h"
#include "UCS_Camera.generated.h"

UCLASS()
class ULTRACONTROLSYSTEM_API AUCS_Camera : public AUCS_ActorBase
{
	GENERATED_BODY()

public:
	

	
	// Sets default values for this actor's properties
	AUCS_Camera();
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;
	

	UFUNCTION(CallInEditor, Category = "UCS")
	void SetCameraActive();
	
	UFUNCTION(CallInEditor, Category = "UCS")
	void TEST();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UCS|MotionControl")
	bool bCanMove=true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UCS|MotionControl")
	bool bCanRotation=true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UCS|MotionControl")
	bool bCanZoom=true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Mechanics", meta = (MakeEditWidget = true))
	FVector TargetLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UCS|MotionControl")
	bool bEnableClamp = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UCS|MotionControl",meta = (EditCondition = "bEnableClamp"))
	FVector2D ClampPitch = FVector2D(-85,-5);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UCS|MotionControl",meta = (EditCondition = "bEnableClamp"))
	FVector2D ClampYaw = FVector2D(-9999,9999);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UCS|MotionControl")
	ECameraType CameraType = ECameraType::Lerp;
	
#if WITH_EDITOR
	// 重写编辑器属性修改事件
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	void OnTargetLocationChanged();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="UCS",Meta = (ExposeOnSpawn = true))
	FUCS_LogicStruct Info;
};
