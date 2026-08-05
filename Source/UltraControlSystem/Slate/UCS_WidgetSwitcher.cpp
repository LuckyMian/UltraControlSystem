// Fill out your copyright notice in the Description page of Project Settings.

#include "UCS_WidgetSwitcher.h"

#include "UCS_MenuBase.h"

namespace
{
	const FString* FindMenuKey(const UUCS_MenuBase* Menu)
	{
		return Menu != nullptr ? Menu->Logic.LogicMap.Find(TEXT("Key")) : nullptr;
	}
}

bool UUCS_WidgetSwitcher::SetActiveWidgetByKey(const FString& Key)
{
	const int32 MatchingIndex = FindWidgetIndexByKey(Key);
	if (MatchingIndex == INDEX_NONE)
	{
		return false;
	}

	SetActiveWidgetIndex(MatchingIndex);
	return true;
}

bool UUCS_WidgetSwitcher::SetActiveWidgetByLogic(const FUCS_LogicStruct& LogicInput)
{
	const FString* Key = LogicInput.LogicMap.Find(TEXT("Key"));
	if (Key == nullptr)
	{
		return false;
	}

	const int32 MatchingIndex = FindWidgetIndexByKey(*Key);
	if (MatchingIndex == INDEX_NONE)
	{
		return false;
	}

	UUCS_MenuBase* MatchingMenu = Cast<UUCS_MenuBase>(GetChildAt(MatchingIndex));
	if (!IsValid(MatchingMenu))
	{
		return false;
	}

	SetActiveWidgetIndex(MatchingIndex);
	return MatchingMenu->ShowPage(LogicInput);
}

int32 UUCS_WidgetSwitcher::FindWidgetIndexByKey(const FString& Key) const
{
	if (Key.IsEmpty())
	{
		return INDEX_NONE;
	}

	for (int32 ChildIndex = 0; ChildIndex < GetChildrenCount(); ++ChildIndex)
	{
		const UUCS_MenuBase* Menu = Cast<UUCS_MenuBase>(GetChildAt(ChildIndex));
		const FString* MenuKey = FindMenuKey(Menu);
		if (MenuKey != nullptr && MenuKey->Equals(Key, ESearchCase::CaseSensitive))
		{
			return ChildIndex;
		}
	}

	return INDEX_NONE;
}

UUCS_MenuBase* UUCS_WidgetSwitcher::GetWidgetByKey(const FString& Key) const
{
	const int32 MatchingIndex = FindWidgetIndexByKey(Key);
	return MatchingIndex != INDEX_NONE
		? Cast<UUCS_MenuBase>(GetChildAt(MatchingIndex))
		: nullptr;
}

FString UUCS_WidgetSwitcher::GetActiveWidgetKey() const
{
	const UUCS_MenuBase* ActiveMenu = Cast<UUCS_MenuBase>(GetActiveWidget());
	const FString* Key = FindMenuKey(ActiveMenu);
	return Key != nullptr ? *Key : FString();
}

#if WITH_EDITOR
const FText UUCS_WidgetSwitcher::GetPaletteCategory()
{
	return NSLOCTEXT("UltraControlSystem", "PanelPaletteCategory", "PANEL");
}
#endif
