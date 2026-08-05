// Fill out your copyright notice in the Description page of Project Settings.


#include "UCS_Widget.h"

#include "UCS_GameInstanceSubsystem.h"
#include "Components/WidgetComponent.h"


// Sets default values
AUCS_Widget::AUCS_Widget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DummyRoot;


	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));

	Widget->SetupAttachment(RootComponent);
	
	Widget->SetWidgetSpace(EWidgetSpace::Screen); 
	//Widget->SetDrawSize(FVector2D(200.f, 100.f)); // 设置 3D UI 渲染分辨率大小
	//Widget->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 默认关闭碰撞
	
	Info.InfoMap.Add(TEXT("CnName"), TEXT(""));
	Info.InfoMap.Add(TEXT("EnName"), TEXT(""));
	Info.InfoMap.Add(TEXT("WidgetSize"), TEXT("1"));
	Info.InfoMap.Add(TEXT("WidgetPivot"), TEXT("(X=0.500000,Y=1.000000)"));
	Info.InfoMap.Add(TEXT("CanClick"), TEXT("0"));
}

// Called when the game starts or when spawned
void AUCS_Widget::BeginPlay()
{
	Super::BeginPlay();
	if (GameInstance)
	{
		GameInstance->Widgets.Add(this);
	}
	this->SetActorHiddenInGame(true);
	this->Widget->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
	this->Tags.Append(Groups);
}



// Called every frame
void AUCS_Widget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUCS_Widget::SetScreenSpaceWidgetScaleAndPivot(UWidgetComponent* TargetComponent, FVector2D NewScale2D,
	FVector2D NewPivot2D)
{
	if (!TargetComponent) return;

	// 1. 严格执行数学转换公式：
	// EnginePivot = Center + (VisualPivot - Center) * Scale
	FVector2D BaseCenter(0.5f, 0.5f);
	FVector2D EnginePivot = BaseCenter + (NewPivot2D - BaseCenter) * NewScale2D;

	// 2. 将换算出来的原本坐标系下的 Pivot 直接设置给 3D 组件
	TargetComponent->SetPivot(EnginePivot);

	// 3. 将 2D 缩放应用到内部真正的 2D Widget 上
	if (UUserWidget* InternalUserWidget = TargetComponent->GetUserWidgetObject())
	{
		FWidgetTransform Transform = InternalUserWidget->GetRenderTransform();
		Transform.Scale = NewScale2D;
		Transform.Translation = FVector2D::ZeroVector; // 保持 0，不使用任何位移干扰

		// 内部 2D 自身的 Pivot 必须同样固定在中心 (0.5, 0.5)，以配合外面组件的缩放中心计算
		InternalUserWidget->SetRenderTransformPivot(BaseCenter);
		InternalUserWidget->SetRenderTransform(Transform);
	}
}

bool AUCS_Widget::ShowSelf_Implementation(FUCS_LogicStruct LogicInput)
{
	this->SetActorHiddenInGame(false);
	this->Widget->GetWidget()->SetVisibility(ESlateVisibility::Visible);
	return true;
}

