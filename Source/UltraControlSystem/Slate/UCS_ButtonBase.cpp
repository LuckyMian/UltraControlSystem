// Fill out your copyright notice in the Description page of Project Settings.

#include "UCS_ButtonBase.h"
#include "UCS_FunctionLibrary.h"

#if WITH_EDITOR
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "WidgetBlueprint.h"
#endif

UUCS_ButtonBase::UUCS_ButtonBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUCS_ButtonBase::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	// UButton is a UWidget rather than an AActor, so it has no BeginPlay.
	// Bind after its underlying Slate widget becomes available instead.
	if (IsDesignTime())
	{
		return;
	}

	OnPressed.AddUniqueDynamic(this, &UUCS_ButtonBase::HandleOnPressed);

	const bool bBound =
		OnPressed.IsAlreadyBound(this, &UUCS_ButtonBase::HandleOnPressed);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[%s] HandleOnPressed binding: %s"),
		*GetNameSafe(this),
		bBound ? TEXT("Success") : TEXT("Failed")
	);
}

#if WITH_EDITOR
void UUCS_ButtonBase::SetName()
{
	const FString* Key = Logic.LogicMap.Find(TEXT("Key"));
	if (Key == nullptr)
	{
		return;
	}

	const FString DisplayName = FString::Printf(TEXT("UCS_Button_%s"), **Key);
	Modify();
	SetDisplayLabel(DisplayName);

	UWidgetBlueprint* WidgetBlueprint = GetTypedOuter<UWidgetBlueprint>();
	if (WidgetBlueprint == nullptr)
	{
		if (UUserWidget* OwningUserWidget = GetTypedOuter<UUserWidget>())
		{
			WidgetBlueprint = Cast<UWidgetBlueprint>(OwningUserWidget->GetClass()->ClassGeneratedBy);
		}
	}

	if (WidgetBlueprint != nullptr && WidgetBlueprint->WidgetTree != nullptr)
	{
		if (UWidget* TemplateWidget = WidgetBlueprint->WidgetTree->FindWidget(GetFName()))
		{
			WidgetBlueprint->Modify();
			TemplateWidget->Modify();
			TemplateWidget->SetDisplayLabel(DisplayName);
		}
	}
}
#endif

void UUCS_ButtonBase::HandleOnPressed()
{
	if (UUCS_GameInstanceSubsystem* GameInstanceSubsystem = UUCS_FunctionLibrary::Get_UCS_GameInstanceSubsystem(this))
	{
		GameInstanceSubsystem->CallMessanger(Logic);
		GameInstanceSubsystem->CallMasterMenuByMessanger(Logic);
	}
}
