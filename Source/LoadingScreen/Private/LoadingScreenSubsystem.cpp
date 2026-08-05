// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenSubsystem.h"

#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"

void ULoadingScreenSubsystem::OpenLevelWithLoadScreen(FName MapPackageName)
{
	if (LoadMapPackageName != TEXT(""))
	{
		UE_LOG(LogTemp, Warning, TEXT("加载任务未完成禁止重复加载！"));
		return;
	}
	LoadMapPackageName = MapPackageName;
	UGameplayStatics::OpenLevel(this, FName(LoadingSetting->TransitionMap.GetAssetName()));

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &ULoadingScreenSubsystem::OnCheckInTransitionMap, 0.1);
}

void ULoadingScreenSubsystem::K2_OpenLevelWithLoadScreen(TSoftObjectPtr<UWorld> Map,
	TSoftObjectPtr<UWorld> TransitionMap, TSoftClassPtr<ULoadScreenUserWidget> LoadScreenUserWidgetClass)
{
	if (Map.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("地图资产不存在，请检查设置内容！"));
		return;
	}
	if (!LoadingSetting)
	{
		LoadingSetting = NewObject<ULoadingSetting>(this);
		LoadingSetting->TransitionMap = TransitionMap;
		LoadingSetting->LoadScreenUserWidgetClass = LoadScreenUserWidgetClass;
	}
	OpenLevelWithLoadScreen(FName(Map.GetLongPackageName()));
}

float ULoadingScreenSubsystem::GetLoadPercent()
{
	if (LoadMapPackageName.IsNone())
	{
		return 0;
	}
	float Percent = GetAsyncLoadPercentage(LoadMapPackageName);
	return Percent;
}

void ULoadingScreenSubsystem::OnCheckInTransitionMap()
{
	if (UGameplayStatics::GetCurrentLevelName(this) != LoadingSetting->TransitionMap.GetAssetName())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &ULoadingScreenSubsystem::OnCheckInTransitionMap, 0.01);
		return;
	}
	
	if (!LoadScreenUserWidget)
	{
		FStreamableManager m_streamableManager;
		TSubclassOf<ULoadScreenUserWidget> LsClass = m_streamableManager.LoadSynchronous<ULoadScreenUserWidget>(LoadingSetting->LoadScreenUserWidgetClass);
		LoadScreenUserWidget = CreateWidget<ULoadScreenUserWidget>(GetGameInstance(), LsClass);
	}
	if (LoadScreenUserWidget)
	{
		LoadScreenUserWidget->AddToViewport();
	}
	
	//启动异步加载目标地图
	LoadPackageAsync(LoadMapPackageName.ToString(), FLoadPackageAsyncDelegate::CreateLambda([this](const FName& PackageName, UPackage* /*LoadedPackage*/, EAsyncLoadingResult::Type Result/*Result*/)
	{
		if (Result == EAsyncLoadingResult::Succeeded)
		{
			UGameplayStatics::OpenLevel(this, PackageName);
			LoadMapPackageName = NAME_None;
			LoadScreenUserWidget->RemoveFromParent();
		}
	}));
}

