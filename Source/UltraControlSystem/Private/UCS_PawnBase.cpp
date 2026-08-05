// Fill out your copyright notice in the Description page of Project Settings.


#include "UCS_PawnBase.h"

#include "../Actor/UCS_Camera.h"
#include "UCS_FunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "UCS_PlayerController.h"

// Sets default values
AUCS_PawnBase::AUCS_PawnBase()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(SceneRoot);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

bool AUCS_PawnBase::MoveInterface_Implementation(const FVector2D MoveInput)
{
	if (bCanMove)
	{
		FRotator ControlRotation = FRotator(0.f, PC->GetControlRotation().Yaw, 0.f);

		FVector ForwardDir = UKismetMathLibrary::GetForwardVector(ControlRotation);
		FVector RightDir   = UKismetMathLibrary::GetRightVector(ControlRotation);

		FVector MovementVector = ForwardDir * MoveInput.X + RightDir * MoveInput.Y;
		this->AddMovementInput(MovementVector);
		bLockMove = false;
		return true;
	}
	return false;
	
}

bool AUCS_PawnBase::RotationInterface_Implementation(const FVector2D MoveInput)
{
	if (bCanRotation)
	{
		FString print = MoveInput.ToString();
		//UE_LOG(LogTemp, Log, TEXT("%s"), *print);
        
		// 获取控制当前 Pawn 的控制器和全局旋转速度
		float GlobalRotationSpeed = GlobalSettings->GlobalSettings.GlobalRotationSpeed;
        
		// 1. 计算原始的 Pitch 和 Yaw
		float TargetPitch = GlobalRotationSpeed * MoveInput.Y + PC->GetControlRotation().Pitch;
		float TargetYaw = GlobalRotationSpeed * MoveInput.X + PC->GetControlRotation().Yaw;
		
		// FMath::Clamp(当前值, 最小值, 最大值)
		// 确保 ClampPitch.X 是较小值（如 -5.0f），ClampPitch.Y 是较大值（如 85.0f）
		TargetPitch = FMath::Clamp(TargetPitch, ClampPitch.X, ClampPitch.Y);
		TargetYaw   = FMath::Clamp(TargetYaw, ClampYaw.X, ClampYaw.Y);
		
		// 3. 应用旋转
		PC->SetControlRotation(FRotator(TargetPitch, TargetYaw, 0.0f));
        
		bLockRotation = false;
		return true;
	}
	return false;
}

bool AUCS_PawnBase::ZoomInterface_Implementation(const float MoveInput)
{
	if (bCanZoom)
	{
		// 设置移动速度
		float ZoomSpeed = GlobalSettings->GlobalSettings.GlobalZoomSpeed * -50.0f;
		float ZoomVlaue = MoveInput* ZoomSpeed;
		NewArmLength = SpringArm->TargetArmLength + ZoomVlaue;
		return true;
	}
	return false;
}

bool AUCS_PawnBase::FocusPoiInterface_Implementation(AActor* UCS_Actor)
{
	AUCS_Camera* Camera_Local = Cast<AUCS_Camera>(UCS_Actor);
	if (Camera_Local)
	{
		NewArmLength = Camera_Local->SpringArm->TargetArmLength;
		TargetCameraLocation = Camera_Local->GetActorLocation();
		TargetCameraRotation = Camera_Local->SpringArm->GetRelativeRotation();
		bLockRotation = true;
		bLockMove = true;
		bCanMove = Camera_Local->bCanMove;
		bCanRotation = Camera_Local->bCanRotation;
		bCanZoom = Camera_Local->bCanZoom;
		ClampPitch = Camera_Local->ClampPitch;
		ClampYaw = Camera_Local->ClampYaw;
		CameraType = Camera_Local->CameraType;

		if (CameraType == ECameraType::Flash)
		{
			AUCS_PlayerController* MyPC = Cast<AUCS_PlayerController>(PC);
			if (MyPC)
			{
				// 在这里调用你自定义控制器的函数或变量
				MyPC->PlayBlackScreenTimeline();
			}
		}
		return true;
	}
	return false;
}




void AUCS_PawnBase::LerpTick(float DeltaTime)
{
	if (!FMath::IsNearlyEqual(NewArmLength, SpringArm->TargetArmLength, 1.00f))
	{
		SpringArm->TargetArmLength = FMath::FInterpTo(                                                   
		SpringArm->TargetArmLength,  // 当前值                                                       
		NewArmLength,                 // 目标值                                                      
		DeltaTime,                                                                                 
		(CameraType == ECameraType::Lerp) ? 1.0f : 0.0f                          // 插值速度
	);
	}
	if (bLockMove)
	{
		if (!this->GetActorLocation().Equals(TargetCameraLocation, 1.00f))
		{
			FVector Location = FMath::VInterpTo(                                                   
				this->GetActorLocation(),                                                          
				TargetCameraLocation,                                                                            
				DeltaTime,                                                                                 
				(CameraType == ECameraType::Lerp) ? 1.0f : 0.0f
			);
			SpringArm->bEnableCameraLag = false;
			this->SetActorLocation(Location);
		}
		else
		{
			bLockMove = false;
			SpringArm->bEnableCameraLag = true;
		}
	}
	if (bLockRotation)
	{
		if (!PC->GetControlRotation().Equals(TargetCameraRotation, 1.00f))
		{
			FRotator Rotation = FMath::RInterpTo(                                                   
				PC->GetControlRotation(),                                                          
				TargetCameraRotation,                                                                            
				DeltaTime,                                                                                 
				(CameraType == ECameraType::Lerp) ? 1.0f : 0.0f
			);
			PC->SetControlRotation(Rotation);
		}
		else
		{
			bLockRotation = false;
		}
	}

}

// Called when the game starts or when spawned
void AUCS_PawnBase::BeginPlay()
{
	
	PC = Cast<APlayerController>(GetController());
	//存全局子系统变量
	GameInstance = UUCS_FunctionLibrary::Get_UCS_GameInstanceSubsystem(this);
	GameInstance->UCS_Pawn = this;
	//存全局设置变量
	GlobalSettings = UUCS_FunctionLibrary::Get_UCS_GlobalSettings(this);
	
	PC->SetShowMouseCursor(true); //显示鼠标
	Super::BeginPlay();
	
}

// Called every frame
void AUCS_PawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LerpTick(DeltaTime);
	// 1. 获取并清空当前的输入向量（Consume 之后该向量会重置为 0）
	FVector PendingInput = ConsumeMovementInputVector();

	// 2. 检查是否有输入，避免不必要的计算
	if (!PendingInput.IsNearlyZero())
	{
		// 设置移动速度
		float MoveSpeed = GlobalSettings->GlobalSettings.GlobalMovementSpeed;
		
		// 3. 计算这一帧应该移动的位移量
		// 位移 = 方向 * 速度 * 时间
		FVector DeltaLocation = PendingInput.GetSafeNormal() * MoveSpeed * DeltaTime ;

		// 4. 应用位移
		// bSweep 为 true 可以开启简单的碰撞检测（遇到墙会停下）
		AddActorWorldOffset(DeltaLocation, true);
	}
}




