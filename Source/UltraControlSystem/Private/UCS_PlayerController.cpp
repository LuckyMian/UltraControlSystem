// Fill out your copyright notice in the Description page of Project Settings.

#include "UCS_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

//slate
#include "UCS_FunctionLibrary.h"
#include "../Slate/SBlackScreen.h"

AUCS_PlayerController::AUCS_PlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCFinder(
		TEXT("/UltraControlSystem/GameMode/Input/UCS_IMC_PawnInputBase.UCS_IMC_PawnInputBase"));
	if (IMCFinder.Succeeded())
	{
		DefaultMappingContext = IMCFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAMoveFinder(
		TEXT("/UltraControlSystem/GameMode/Input/IA_Move.IA_Move"));
	if (IAMoveFinder.Succeeded())
	{
		MoveAction = IAMoveFinder.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> IARotationAction(
	TEXT("/UltraControlSystem/GameMode/Input/IA_Rotation.IA_Rotation"));
	if (IARotationAction.Succeeded())
	{
		RotationAction = IARotationAction.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> IAZoomAction(
	TEXT("/UltraControlSystem/GameMode/Input/IA_Zoom.IA_Zoom"));
	if (IAZoomAction.Succeeded())
	{
		ZoomAction = IAZoomAction.Object;
	}
}
void AUCS_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	GameInstance = UUCS_FunctionLibrary::Get_UCS_GameInstanceSubsystem(this);
	GameInstance->UCS_PlayerController = this;
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			SetupInputComponent();
		}
	}
	
	if (GEngine && GEngine->GameViewport)
	{
		if (!UCSWidget)
		{
			UCSWidget = UUCS_FunctionLibrary::Load_UCS_UMG_ByConfig(this);
		}

		if (UCSWidget && !UCSWidget->IsInViewport())
		{
			UCSWidget->AddToViewport(1001);
		}
	}
}

void AUCS_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveAction)
		{
			EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUCS_PlayerController::OnMove);
		}
		if (RotationAction)
		{
			EIC->BindAction(RotationAction, ETriggerEvent::Triggered, this, &AUCS_PlayerController::OnRotation);
		}
		if (RotationAction)
		{
			EIC->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AUCS_PlayerController::OnZoom);
		}
	}
}

void AUCS_PlayerController::OnMove(const FInputActionValue& Value)
{
	const FVector2D MoveInput = Value.Get<FVector2D>();
	if (IUCS_Interface* PawnInterface = Cast<IUCS_Interface>(GetPawn()))
	{
		PawnInterface->MoveInterface_Implementation(MoveInput);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Pawn Controller could not be found"));
	}
}

void AUCS_PlayerController::OnRotation(const FInputActionValue& Value)
{
	const FVector2D Rotation = Value.Get<FVector2D>();
	if (IUCS_Interface* PawnInterface = Cast<IUCS_Interface>(GetPawn()))
	{
		PawnInterface->RotationInterface_Implementation(Rotation);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Pawn Controller could not be found"));
	}
}

void AUCS_PlayerController::OnZoom(const FInputActionValue& Value)
{
	const float ZoomInput = Value.Get<float>();
	if (IUCS_Interface* PawnInterface = Cast<IUCS_Interface>(GetPawn()))
	{
		PawnInterface->ZoomInterface_Implementation(ZoomInput);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Pawn Controller could not be found"));
	}
}

void AUCS_PlayerController::PlayBlackScreenTimeline()
{
	if (GEngine && GEngine->GameViewport)
	{
		if (!BlackScreenWidget)
		{
			// 创建我们自定义的控件
			BlackScreenWidget = SNew(SBlackScreen);
			
			// 添加到全屏视口
			GEngine->GameViewport->AddViewportWidgetContent(BlackScreenWidget.ToSharedRef(), 1000);
		}
		else
		{
			BlackScreenWidget->StartFadeAnimation();
		}
		

	
	}
}


