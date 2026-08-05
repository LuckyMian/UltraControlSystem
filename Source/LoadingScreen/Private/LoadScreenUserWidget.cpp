// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadScreenUserWidget.h"

#include "LoadingScreenSubsystem.h"


void ULoadScreenUserWidget::OnReceiveLoadPercent(float Percent)
{
	K2_OnReceiveLoadPercent(Percent);
}


void ULoadScreenUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (IsInViewport())
	{
		//更新进度信息
		float Schedule = GetWorld()->GetGameInstance()->GetSubsystem<ULoadingScreenSubsystem>()->GetLoadPercent();
		OnReceiveLoadPercent(Schedule);
		
	}
}