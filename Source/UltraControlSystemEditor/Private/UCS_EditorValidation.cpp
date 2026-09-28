#include "UCS_EditorValidation.h"

#include "../../UltraControlSystem/Actor/UCS_Camera.h"
#include "../../UltraControlSystem/Actor/UCS_Widget.h"
#include "EngineUtils.h"
#include "Logging/MessageLog.h"
#include "Logging/TokenizedMessage.h"
#include "Misc/UObjectToken.h"
#include "UCS_ProjectSettings.h"

#define LOCTEXT_NAMESPACE "UCSSceneValidator"

namespace
{
void AddIssue(TArray<FUCSValidationIssue>& Issues, EUCSValidationSeverity Severity, const FText& Message, AActor* Actor)
{
	Issues.Add({ Severity, Message, Actor });
}

bool HasRequiredField(const FUCS_LogicStruct& Info, const FUCSLogicFieldDefinition& Definition)
{
	const TMap<FString, FString>& Map = Definition.Target == EUCSLogicMapTarget::InfoMap ? Info.InfoMap : Info.LogicMap;
	const FString* Value = Map.Find(Definition.Name.ToString());
	return Value != nullptr && !Value->TrimStartAndEnd().IsEmpty();
}
}

TArray<FUCSValidationIssue> FUCSSceneValidator::Validate(UWorld* World)
{
	TArray<FUCSValidationIssue> Issues;
	if (!IsValid(World))
	{
		AddIssue(Issues, EUCSValidationSeverity::Error, LOCTEXT("NoWorld", "当前没有打开可编辑的关卡。"), nullptr);
		return Issues;
	}

	TMap<FString, TArray<AUCS_Camera*>> CamerasByKey;
	TSet<FString> CameraKeys;
	TArray<AUCS_Widget*> Widgets;
	const TArray<FUCSLogicFieldDefinition>& Schema = GetDefault<UUCS_ProjectSettings>()->LogicSchema;

	for (TActorIterator<AUCS_Camera> It(World); It; ++It)
	{
		AUCS_Camera* Camera = *It;
		const FString Key = Camera->Info.LogicMap.FindRef(TEXT("Key")).TrimStartAndEnd();
		if (Key.IsEmpty())
		{
			AddIssue(Issues, EUCSValidationSeverity::Error, LOCTEXT("CameraKeyEmpty", "镜头的 LogicMap.Key 不能为空。"), Camera);
		}
		else
		{
			const FString Normalized = Key.ToLower();
			CamerasByKey.FindOrAdd(Normalized).Add(Camera);
			CameraKeys.Add(Normalized);
		}

		if (Camera->ClampPitch.X > Camera->ClampPitch.Y || Camera->ClampYaw.X > Camera->ClampYaw.Y)
		{
			AddIssue(Issues, EUCSValidationSeverity::Error, LOCTEXT("ClampInvalid", "镜头限制范围的最小值不能大于最大值。"), Camera);
		}

		if (Camera->Info.InfoMap.FindRef(TEXT("CnName")).TrimStartAndEnd().IsEmpty()
			&& Camera->Info.InfoMap.FindRef(TEXT("EnName")).TrimStartAndEnd().IsEmpty())
		{
			AddIssue(Issues, EUCSValidationSeverity::Warning, LOCTEXT("CameraNameEmpty", "镜头未填写中文名称 CnName 或英文名称 EnName。"), Camera);
		}

		for (const FUCSLogicFieldDefinition& Definition : Schema)
		{
			if (Definition.bRequired && !HasRequiredField(Camera->Info, Definition))
			{
				AddIssue(Issues, EUCSValidationSeverity::Error,
					FText::Format(LOCTEXT("RequiredFieldMissing", "缺少必填字段“{0}”。"), FText::FromName(Definition.Name)), Camera);
			}
		}
	}

	for (const TPair<FString, TArray<AUCS_Camera*>>& Pair : CamerasByKey)
	{
		if (Pair.Value.Num() > 1)
		{
			for (AUCS_Camera* Camera : Pair.Value)
			{
				AddIssue(Issues, EUCSValidationSeverity::Error,
					FText::Format(LOCTEXT("DuplicateCameraKey", "镜头 Key“{0}”重复（不区分大小写）。"), FText::FromString(Pair.Key)), Camera);
			}
		}
	}

	TSet<FString> LinkedCameraKeys;
	for (TActorIterator<AUCS_Widget> It(World); It; ++It)
	{
		AUCS_Widget* Widget = *It;
		Widgets.Add(Widget);
		if (!Widget->WidgetClass)
		{
			AddIssue(Issues, EUCSValidationSeverity::Error, LOCTEXT("WidgetClassMissing", "场景 Widget 尚未指定 WidgetClass。"), Widget);
		}
		if (Widget->Groups.IsEmpty())
		{
			AddIssue(Issues, EUCSValidationSeverity::Warning, LOCTEXT("WidgetGroupsEmpty", "场景 Widget 的 Groups 为空。"), Widget);
		}
		for (const FName Group : Widget->Groups)
		{
			const FString Normalized = Group.ToString().TrimStartAndEnd().ToLower();
			if (!CameraKeys.Contains(Normalized))
			{
				AddIssue(Issues, EUCSValidationSeverity::Warning,
					FText::Format(LOCTEXT("OrphanWidgetGroup", "Widget Group“{0}”没有匹配的镜头。"), FText::FromName(Group)), Widget);
			}
			else
			{
				LinkedCameraKeys.Add(Normalized);
			}
		}
	}

	for (const TPair<FString, TArray<AUCS_Camera*>>& Pair : CamerasByKey)
	{
		if (!LinkedCameraKeys.Contains(Pair.Key))
		{
			for (AUCS_Camera* Camera : Pair.Value)
			{
				AddIssue(Issues, EUCSValidationSeverity::Warning, LOCTEXT("CameraWithoutWidget", "镜头没有关联任何场景 Widget Group。"), Camera);
			}
		}
	}

	return Issues;
}

void FUCSSceneValidator::PublishToMessageLog(const TArray<FUCSValidationIssue>& Issues, bool bOpenLog)
{
	FMessageLog Log(TEXT("UCS"));
	Log.NewPage(LOCTEXT("ValidationPage", "UCS Scene Validation"));
	if (Issues.IsEmpty())
	{
		Log.Info(LOCTEXT("ValidationSuccess", "UCS 场景校验通过，未发现问题。"));
	}
	for (const FUCSValidationIssue& Issue : Issues)
	{
		EMessageSeverity::Type Severity = EMessageSeverity::Warning;
		if (Issue.Severity == EUCSValidationSeverity::Error) Severity = EMessageSeverity::Error;
		else if (Issue.Severity == EUCSValidationSeverity::Info) Severity = EMessageSeverity::Info;

		TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create(Severity, Issue.Message);
		if (Issue.Actor.IsValid())
		{
			Message->AddToken(FUObjectToken::Create(Issue.Actor.Get()));
		}
		Log.AddMessage(Message);
	}
	Log.Notify(LOCTEXT("ValidationNotification", "UCS 场景校验已完成。"), EMessageSeverity::Info, bOpenLog);
}

#undef LOCTEXT_NAMESPACE
