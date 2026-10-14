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
	SpringArm->bDrawDebugLagMarkers = false;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritRoll = false;

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
		const float CameraArmLength = FMath::Max(0.0f, Camera_Local->SpringArm->TargetArmLength);
		const float MinArmLength = Camera_Local->GetTransitionMinArmLength();
		const float MaxArmLength = Camera_Local->GetTransitionMaxArmLength();
		TargetCameraLocation = Camera_Local->GetActorLocation();
		// The pawn spring arm follows the controller's world rotation. Using the
		// target arm's relative rotation drops the camera actor's rotation and
		// makes web-created cameras jump to a level view.
		TargetCameraRotation = Camera_Local->SpringArm->GetComponentRotation();
		TargetCameraRotation.Roll = 0.0f;
		bLockRotation = true;
		bLockMove = true;
		bCanMove = Camera_Local->bCanMove;
		bCanRotation = Camera_Local->bCanRotation;
		bCanZoom = Camera_Local->bCanZoom;
		ClampPitch = Camera_Local->ClampPitch;
		ClampYaw = Camera_Local->ClampYaw;
		CameraType = Camera_Local->CameraType;
		TransitionSpeedMultiplier = FMath::Max(Camera_Local->TransitionSpeedMultiplier, 0.01f);
		HitchcockReferenceFieldOfView = FMath::Clamp(Camera_Local->Camera->FieldOfView, 5.0f, 170.0f);

		switch (CameraType)
		{
		case ECameraType::PullBack:
			// Teleport to the camera first, then move from its configured arm
			// length to the transition minimum.
			SpringArm->TargetArmLength = CameraArmLength;
			NewArmLength = MinArmLength;
			break;
		case ECameraType::MinToArmLength:
			// Teleport to the transition minimum, then move out to the arm
			// length configured on the target camera.
			SpringArm->TargetArmLength = MinArmLength;
			NewArmLength = CameraArmLength;
			break;
		case ECameraType::HitchcockPullBack:
			// Start close with a wide FOV, then pull back to the camera's
			// configured arm length and target FOV.
			SpringArm->TargetArmLength = MinArmLength;
			NewArmLength = CameraArmLength;
			break;
		case ECameraType::PushIn:
		case ECameraType::HitchcockPushIn:
			// Teleport to the camera first, then move from the transition maximum
			// to the arm length configured on the target camera.
			SpringArm->TargetArmLength = MaxArmLength;
			NewArmLength = CameraArmLength;
			break;
		default:
			NewArmLength = CameraArmLength;
			break;
		}

		const bool bUsesHitchcockZoom = CameraType == ECameraType::HitchcockPullBack
			|| CameraType == ECameraType::HitchcockPushIn;
		if (bUsesHitchcockZoom)
		{
			HitchcockReferenceArmLength = FMath::Max(NewArmLength, 1.0f);
			UpdateHitchcockFieldOfView();
		}
		else
		{
			Camera->SetFieldOfView(HitchcockReferenceFieldOfView);
		}

		const bool bUsesBlackScreen = CameraType == ECameraType::Flash
			|| CameraType == ECameraType::PullBack
			|| CameraType == ECameraType::PushIn
			|| CameraType == ECameraType::MinToArmLength;
		if (bUsesBlackScreen)
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
void AUCS_PawnBase::UpdateHitchcockFieldOfView()
{
	if (!Camera || !SpringArm)
	{
		return;
	}

	const float CurrentArmLength = FMath::Max(SpringArm->TargetArmLength, 1.0f);
	const float ReferenceArmLength = FMath::Max(HitchcockReferenceArmLength, 1.0f);
	const float ReferenceFov = FMath::Clamp(HitchcockReferenceFieldOfView, 5.0f, 170.0f);
	const float ReferenceHalfFovRadians = FMath::DegreesToRadians(ReferenceFov * 0.5f);
	const float ScaledHalfFovTangent = ReferenceArmLength / CurrentArmLength
		* FMath::Tan(ReferenceHalfFovRadians);
	const float HitchcockFov = FMath::RadiansToDegrees(2.0f * FMath::Atan(ScaledHalfFovTangent));
	Camera->SetFieldOfView(FMath::Clamp(HitchcockFov, 5.0f, 170.0f));
}

void AUCS_PawnBase::LerpTick(float DeltaTime)
{
	const float TransitionSpeed = FMath::Max(TransitionSpeedMultiplier, 0.01f);
	if (PC)
	{
		FRotator HorizontalControlRotation = PC->GetControlRotation();
		if (!FMath::IsNearlyZero(HorizontalControlRotation.Roll))
		{
			HorizontalControlRotation.Roll = 0.0f;
			PC->SetControlRotation(HorizontalControlRotation);
		}
	}

	if (!FMath::IsNearlyEqual(NewArmLength, SpringArm->TargetArmLength, 1.00f))
	{
		const bool bInterpolateArmLength = CameraType == ECameraType::Lerp
			|| CameraType == ECameraType::PullBack
			|| CameraType == ECameraType::PushIn
			|| CameraType == ECameraType::MinToArmLength
			|| CameraType == ECameraType::HitchcockPullBack
			|| CameraType == ECameraType::HitchcockPushIn;
		SpringArm->TargetArmLength = FMath::FInterpTo(                                                   
		SpringArm->TargetArmLength,  // 当前值                                                       
		NewArmLength,                 // 目标值                                                      
		DeltaTime,                                                                                 
		bInterpolateArmLength ? TransitionSpeed : 0.0f              // 插值速度
	);
		if (FMath::IsNearlyEqual(NewArmLength, SpringArm->TargetArmLength, 1.00f))
		{
			SpringArm->TargetArmLength = NewArmLength;
		}
	}

	if (CameraType == ECameraType::HitchcockPullBack
		|| CameraType == ECameraType::HitchcockPushIn)
	{
		UpdateHitchcockFieldOfView();
	}

	if (bLockMove)
	{
		if (!this->GetActorLocation().Equals(TargetCameraLocation, 1.00f))
		{
			FVector Location = FMath::VInterpTo(                                                   
				this->GetActorLocation(),                                                          
				TargetCameraLocation,                                                                            
				DeltaTime,                                                                                 
				(CameraType == ECameraType::Lerp) ? TransitionSpeed : 0.0f
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
	if (bLockRotation && PC)
	{
		if (!PC->GetControlRotation().Equals(TargetCameraRotation, 1.00f))
		{
			FRotator Rotation = FMath::RInterpTo(                                                   
				PC->GetControlRotation(),                                                          
				TargetCameraRotation,                                                                            
				DeltaTime,                                                                                 
				(CameraType == ECameraType::Lerp) ? TransitionSpeed : 0.0f
			);
			Rotation.Roll = 0.0f;
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
	Super::BeginPlay();

	// Blueprint component defaults may retain this development-only SpringArm
	// visualization or collision settings. Keep the runtime camera on the exact
	// configured arm length so push/pull and Hitchcock transitions remain visible.
	SpringArm->bDrawDebugLagMarkers = false;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritRoll = false;

	PC = Cast<APlayerController>(GetController());
	//存全局子系统变量
	GameInstance = UUCS_FunctionLibrary::Get_UCS_GameInstanceSubsystem(this);
	if (GameInstance)
	{
		GameInstance->UCS_Pawn = this;
		GameInstance->RegisterUCSReceiver(this);
	}
	//存全局设置变量
	GlobalSettings = UUCS_FunctionLibrary::Get_UCS_GlobalSettings(this);

	if (PC)
	{
		PC->SetShowMouseCursor(true); //显示鼠标
	}
	
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




