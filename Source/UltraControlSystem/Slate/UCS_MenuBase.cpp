// Fill out your copyright notice in the Description page of Project Settings.

#include "UCS_MenuBase.h"

#include "Animation/WidgetAnimation.h"

bool UUCS_MenuBase::ShowPage(FUCS_LogicStruct LogicInput)
{
	Logic = MoveTemp(LogicInput);

	if (PlayPageAnimation())
	{
		return true;
	}

	// Keep existing Blueprint implementations working while widgets migrate to
	// the native PageAnimation binding.
	StartPageAnimation();
	return true;
}

bool UUCS_MenuBase::PlayPageAnimation()
{
	if (!IsValid(PageAnimation))
	{
		return false;
	}

	// Always restart from the beginning when the page becomes active again.
	StopAnimation(PageAnimation);
	PlayAnimation(PageAnimation);
	return true;
}
