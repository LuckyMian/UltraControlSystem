// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UCS_Interface.h"
#include "UCS_ActorBase.generated.h"


class UUCS_GameInstanceSubsystem;

UCLASS()
class ULTRACONTROLSYSTEM_API AUCS_ActorBase : public AActor,public IUCS_Interface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUCS_ActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	UPROPERTY()
	UUCS_GameInstanceSubsystem* GameInstance; //子系统变量
};
