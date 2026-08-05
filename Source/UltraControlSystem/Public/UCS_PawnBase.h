// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UCS_Interface.h"
#include "UCS_GameInstanceSubsystem.h"
#include "UCS_FunctionLibrary.h"
#include "UCS_PawnBase.generated.h"

UCLASS()
class ULTRACONTROLSYSTEM_API AUCS_PawnBase : public APawn, public IUCS_Interface
{
	GENERATED_BODY()

public:
	AUCS_PawnBase();

	virtual bool MoveInterface_Implementation(const FVector2D MoveInput) override;
	virtual bool RotationInterface_Implementation(const FVector2D MoveInput) override;
	virtual bool ZoomInterface_Implementation(const float ZoomInput) override;
	virtual bool FocusPoiInterface_Implementation(AActor* UCS_Actor) override;
	
	UPROPERTY(VisibleAnywhere, Category = "UCS")
	float NewArmLength = 300.0f;
	
	void LerpTick(float DeltaTime);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY()
	UUCS_GameInstanceSubsystem* GameInstance; //子系统变量
	
	UPROPERTY()
	UUCS_GlobalSettings* GlobalSettings;
	
	UPROPERTY()
	FVector TargetCameraLocation;
	
	UPROPERTY()
	FRotator TargetCameraRotation;
	
	UPROPERTY()
	APlayerController* PC;
	
	UPROPERTY()
	bool bLockRotation = false;

	UPROPERTY()
	bool bLockMove = false;
	
	UPROPERTY()
	bool bCanMove=true;
	
	UPROPERTY()
	bool bCanRotation=true;
	
	UPROPERTY()
	bool bCanZoom=true;
	
	UPROPERTY()
	FVector2D ClampPitch = FVector2D(-85,-5);
	
	UPROPERTY()
	FVector2D ClampYaw = FVector2D(-9999,9999);
	
	UPROPERTY()
	ECameraType CameraType = ECameraType::Lerp;
	
	
public:
	virtual void Tick(float DeltaTime) override;
	

	
private:
	
};
