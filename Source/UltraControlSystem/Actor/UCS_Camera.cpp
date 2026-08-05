// Fill out your copyright notice in the Description page of Project Settings.



#include "UCS_Camera.h"

#include "UCS_PawnBase.h"


// Sets default values
AUCS_Camera::AUCS_Camera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(SceneRoot);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bEnableCameraLag = false;
	SpringArm->bDoCollisionTest = false;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetAutoActivate(false);
	Camera->SetActive(false);
	
	TargetLocation = FVector(-300, 0, 0);
	
	
	//Info.InfoMap.Add(TEXT("Key"), TEXT(""));
	
}


void AUCS_Camera::SetCameraActive()
{
	if (Camera)
	{
		Camera->SetActive(!Camera->IsActive());
		
	}
}

void AUCS_Camera::TEST()
{
	UE_LOG(LogTemp, Warning, TEXT("Test UCS Camera"));
	GameInstance->UCS_Pawn->FocusPoiInterface_Implementation(this);
}

#if WITH_EDITOR
void AUCS_Camera::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	// 获取当前被修改的属性名称
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// 比对属性名是否是我们的 TargetLocation
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AUCS_Camera, TargetLocation))
	{
		// 触发你的自定义函数
		OnTargetLocationChanged();
	}
}
#endif

void AUCS_Camera::OnTargetLocationChanged()
{
	
	FRotator ArmRotation = FRotator((-TargetLocation).Rotation().Pitch, (-TargetLocation).Rotation().Yaw, (-TargetLocation).Rotation().Roll);
	SpringArm->SetRelativeRotation(ArmRotation);
	SpringArm->TargetArmLength = TargetLocation.Length();
}


// Called when the game starts or when spawned
void AUCS_Camera::BeginPlay()
{
	Super::BeginPlay();
	
	//存TAG
	FString* Key = Info.LogicMap.Find(TEXT("Key"));
	this->Tags.Add(FName(*Key));
	
	GameInstance->Cameras.Add(this);
}

// Called every frame
void AUCS_Camera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

