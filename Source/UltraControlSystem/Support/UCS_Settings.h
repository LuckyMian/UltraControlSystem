
#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Dom/JsonObject.h"
#include "UCS_Settings.generated.h" 


USTRUCT(BlueprintType)
struct FGlobalSettings
{
	GENERATED_BODY()
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float GlobalMovementSpeed = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float GlobalRotationSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float GlobalZoomSpeed = 1.0f;
};


UCLASS()
class UUCS_GlobalSettings : public UObject
{
	GENERATED_BODY()

public:
	FGlobalSettings GlobalSettings;
	FJsonObjectWrapper Json;

	UUCS_GlobalSettings(); // 声明构造函数
	
};

