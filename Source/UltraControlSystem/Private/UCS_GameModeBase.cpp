// Fill out your copyright notice in the Description page of Project Settings.

#include "UCS_GameModeBase.h"
#include "UCS_PawnBase.h"
#include "UCS_PlayerController.h"

AUCS_GameModeBase::AUCS_GameModeBase()
{
	DefaultPawnClass = AUCS_PawnBase::StaticClass();
	PlayerControllerClass = AUCS_PlayerController::StaticClass();
	
}
