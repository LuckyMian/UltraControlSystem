// Fill out your copyright notice in the Description page of Project Settings.


#include "UCS_WidgetBase.h"



void UUCS_WidgetBase::InitWidget_Implementation()
{
}

bool UUCS_WidgetBase::Initialize()
{
	// 1. 务必先调用父类的 Initialize，如果返回 false 则直接中断
	if (!Super::Initialize())
	{
		return false;
	}

	// 2. 在这里对结构体进行初始化赋值
	Info.InfoMap.Add(TEXT("Key"), TEXT(""));
	Info.InfoMap.Add(TEXT("CnName"), TEXT(""));
	Info.InfoMap.Add(TEXT("EnName"), TEXT(""));
	Info.InfoMap.Add(TEXT("WidgetSize"), TEXT("1"));

	return true;
}
