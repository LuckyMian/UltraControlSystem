#pragma once
#include "CoreMinimal.h"
#include "UCS_LogicStruct.generated.h" 

USTRUCT(BlueprintType)
struct FUCS_LogicStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	TMap<FString, FString> InfoMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	TMap<FString, FString> LogicMap;
	
	// 使用初始化列表赋初值
	FUCS_LogicStruct() 
		: InfoMap({
			{ TEXT("CnName"), TEXT("") },
			{ TEXT("EnName"), TEXT("") },
		  }) , LogicMap({
		  	{ TEXT("Key"), TEXT("") },
		  })
	{
	}
};