// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UCSS_GameInstanceSubsystem.h"
#include "UCSS_LocalPlayerSubsystem.h"
#include "UCSS_TickableGameInstanceSubsystem.h"
#include "UCSS_TickableLocalPlayerSubsystem.h"
#include "UCSS_TickableWorldSubsystem.h"
#include "UCSS_WorldSubsystem.h"

#include "Kismet2/KismetEditorUtilities.h"
#include "UCSS_SubsystemFactory.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ULTRACONTROLSUBSYSTEMEDITOR_API UUCSS_SubsystemFactory: public UFactory
{
	GENERATED_BODY()
public:
	UUCSS_SubsystemFactory()
	{
		bCreateNew=true;
		bEditAfterNew=true;
	}
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
		UObject* Context, FFeedbackContext* Warn, FName CallingContext) override
	{
		// return NewObject<USubsystem>(InParent,InClass,InName,Flags);
		return FKismetEditorUtilities::CreateBlueprint(InClass, InParent, InName, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext);
	}

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
		UObject* Context, FFeedbackContext* Warn) override
	{
		return FactoryCreateNew(InClass,InParent,InName,Flags,Context,Warn,NAME_None);
	}
};


UCLASS(HideCategories=Object)
class ULTRACONTROLSUBSYSTEMEDITOR_API UUCSS_GameInstanceSubsystemFactory : public UUCSS_SubsystemFactory
{
	GENERATED_BODY()

public:
	UUCSS_GameInstanceSubsystemFactory()
	{
		SupportedClass=UUCSS_GameInstanceSubsystem::StaticClass();
	}
};

UCLASS(HideCategories=Object)
class ULTRACONTROLSUBSYSTEMEDITOR_API UUCSS_WorldSubsystemFactory : public UUCSS_SubsystemFactory
{
	GENERATED_BODY()

public:
	UUCSS_WorldSubsystemFactory()
	{
		SupportedClass=UUCSS_WorldSubsystem::StaticClass();
	}
};

UCLASS(HideCategories=Object)
class ULTRACONTROLSUBSYSTEMEDITOR_API UUCSS_LocalPlayerSubsystemFactory : public UUCSS_SubsystemFactory
{
	GENERATED_BODY()

public:
	UUCSS_LocalPlayerSubsystemFactory()
	{
		SupportedClass=UUCSS_LocalPlayerSubsystem::StaticClass();
	}
};



UCLASS(HideCategories=Object)
class ULTRACONTROLSUBSYSTEMEDITOR_API UUCSS_TickableGameInstanceSubsystemFactory : public UUCSS_SubsystemFactory
{
	GENERATED_BODY()

public:
	UUCSS_TickableGameInstanceSubsystemFactory()
	{
		SupportedClass=UUCSS_TickableGameInstanceSubsystem::StaticClass();
	}
};

UCLASS(HideCategories=Object)
class ULTRACONTROLSUBSYSTEMEDITOR_API UUCSS_TickableWorldSubsystemFactory : public UUCSS_SubsystemFactory
{
	GENERATED_BODY()

public:
	UUCSS_TickableWorldSubsystemFactory()
	{
		SupportedClass=UUCSS_TickableWorldSubsystem::StaticClass();
	}
};

UCLASS(HideCategories=Object)
class ULTRACONTROLSUBSYSTEMEDITOR_API UUCSS_TickableLocalPlayerSubsystemFactory : public UUCSS_SubsystemFactory
{
	GENERATED_BODY()

public:
	UUCSS_TickableLocalPlayerSubsystemFactory()
	{
		SupportedClass=UUCSS_TickableLocalPlayerSubsystem::StaticClass();
	}
};
