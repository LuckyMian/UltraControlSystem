#pragma once

#include "CoreMinimal.h"

class AActor;
class UWorld;

enum class EUCSValidationSeverity : uint8
{
	Info,
	Warning,
	Error
};

struct FUCSValidationIssue
{
	EUCSValidationSeverity Severity = EUCSValidationSeverity::Warning;
	FText Message;
	TWeakObjectPtr<AActor> Actor;
};

class FUCSSceneValidator
{
public:
	static TArray<FUCSValidationIssue> Validate(UWorld* World);
	static void PublishToMessageLog(const TArray<FUCSValidationIssue>& Issues, bool bOpenLog);
};
