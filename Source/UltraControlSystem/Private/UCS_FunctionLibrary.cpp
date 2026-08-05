// Fill out your copyright notice in the Description page of Project Settings.


#include "UCS_FunctionLibrary.h"

#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "JsonObjectWrapper.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(UCS_Logs, Log, All);

namespace
{
	const TCHAR* UCSConfigSection = TEXT("/Script/UltraControlSystemEditor.UCS_Config");
	const TCHAR* UCSWidgetPathKey = TEXT("WidgetPath");

	FString ExtractObjectPathFromExportText(const FString& Path)
	{
		FString Result = Path;
		Result.TrimStartAndEndInline();

		int32 FirstQuoteIndex = INDEX_NONE;
		int32 LastQuoteIndex = INDEX_NONE;
		if (Result.FindChar(TEXT('\''), FirstQuoteIndex) && Result.FindLastChar(TEXT('\''), LastQuoteIndex) && LastQuoteIndex > FirstQuoteIndex)
		{
			Result = Result.Mid(FirstQuoteIndex + 1, LastQuoteIndex - FirstQuoteIndex - 1);
			Result.TrimStartAndEndInline();
		}

		return Result;
	}

	FString BuildWidgetGeneratedClassPath(const FString& ConfigPath)
	{
		FString ObjectPath = ExtractObjectPathFromExportText(ConfigPath);
		if (ObjectPath.IsEmpty())
		{
			return FString();
		}

		if (!ObjectPath.Contains(TEXT(".")))
		{
			ObjectPath += TEXT(".") + FPackageName::GetShortName(ObjectPath);
		}

		if (!ObjectPath.EndsWith(TEXT("_C")))
		{
			ObjectPath += TEXT("_C");
		}

		return ObjectPath;
	}

	UClass* LoadUCSWidgetClass(const FString& ConfigPath)
	{
		const FString GeneratedClassPath = BuildWidgetGeneratedClassPath(ConfigPath);
		if (!GeneratedClassPath.IsEmpty())
		{
			if (UClass* WidgetClass = LoadClass<UUCS_MenuBase>(nullptr, *GeneratedClassPath))
			{
				return WidgetClass;
			}
		}

		UObject* LoadedObject = StaticLoadObject(UObject::StaticClass(), nullptr, *ConfigPath);
		if (UBlueprint* Blueprint = Cast<UBlueprint>(LoadedObject))
		{
			return Blueprint->GeneratedClass;
		}

		return Cast<UClass>(LoadedObject);
	}
}

UUCS_GameInstanceSubsystem* UUCS_FunctionLibrary::Get_UCS_GameInstanceSubsystem(const UObject* WorldContextObject)
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return GI->GetSubsystem<UUCS_GameInstanceSubsystem>();
	}
    
	return nullptr;
}

UUCS_GlobalSettings* UUCS_FunctionLibrary::Get_UCS_GlobalSettings(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		return Get_UCS_GameInstanceSubsystem(WorldContextObject)->UCS_GameInstanceSettings;
	}
	return nullptr;
}

FJsonObjectWrapper UUCS_FunctionLibrary::ReadGlobalSettingsJson()
{
	FJsonObjectWrapper ReturnWrapper;

	// 1. 获取路径
	FString FilePath = FPaths::ProjectContentDir() / TEXT("Settings/GlobalSettings.json");

	// 2. 读取文件
	FString FileContent;
	if (FFileHelper::LoadFileToString(FileContent, *FilePath))
	{
		// 3. 核心黑科技：Wrapper 自带了解析字符串的功能，会自动填充内部的 JsonObject
		if (!ReturnWrapper.JsonObjectFromString(FileContent))
		{
			UE_LOG(LogTemp, Error, TEXT("UCS: Json content is invalid!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCS: Failed to load file at: %s"), *FilePath);
	}

	// 4. 返回 Wrapper（如果解析失败，它内部的指针会是空的，蓝图里用 IsValid 节点可以判断）
	return ReturnWrapper;
}

UUCS_MenuBase* UUCS_FunctionLibrary::Load_UCS_UMG_ByConfig(const UObject* WorldContextObject)
{
	FString WidgetPath;
	if (!GConfig || !GConfig->GetString(UCSConfigSection, UCSWidgetPathKey, WidgetPath, GGameIni))
	{
		UE_LOG(LogTemp, Error, TEXT("菜单路径配置不正确: [%s]."), UCSConfigSection);
		return nullptr;
	}

	WidgetPath.TrimStartAndEndInline();
	if (WidgetPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("菜单路径配置为空 [%s]."), UCSConfigSection);
		return nullptr;
	}

	UClass* WidgetClass = LoadUCSWidgetClass(WidgetPath);
	if (!WidgetClass || !WidgetClass->IsChildOf(UUCS_MenuBase::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("菜单路径配置无法完成类型转换  %s"), *WidgetPath);
		return nullptr;
	}

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("菜单无法正确生成,因为世界上下文有误."));
		return nullptr;
	}

	return CreateWidget<UUCS_MenuBase>(World, WidgetClass);
}

void UUCS_FunctionLibrary::PrintUCSLog(FUCS_LogicStruct LogicInput, UObject* Object, FString Type)
{
	const FString ObjectDisplayName = IsValid(Object)
		? UKismetSystemLibrary::GetDisplayName(Object)
		: TEXT("None");

	FString LogicInputText;
	FUCS_LogicStruct::StaticStruct()->ExportText(
		LogicInputText,
		&LogicInput,
		nullptr,
		nullptr,
		PPF_None,
		nullptr);

	UE_LOG(UCS_Logs, Log, TEXT("UObject: %s"), *ObjectDisplayName);
	UE_LOG(UCS_Logs, Log, TEXT("Type: %s"), *Type);
	UE_LOG(UCS_Logs, Log, TEXT("LogicInput: %s"), *LogicInputText);
}
