// Fill out your copyright notice in the Description page of Project Settings.


#include "UCS_ActorBase.h"

#include "UCS_FunctionLibrary.h"


// Sets default values
AUCS_ActorBase::AUCS_ActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AUCS_ActorBase::BeginPlay()
{
	GameInstance = UUCS_FunctionLibrary::Get_UCS_GameInstanceSubsystem(this);
	Super::BeginPlay();
	
}

// Called every frame
void AUCS_ActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

