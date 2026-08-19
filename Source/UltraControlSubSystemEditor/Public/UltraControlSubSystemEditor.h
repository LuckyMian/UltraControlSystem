// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UCSS_AssetTypeActions_Subsystem.h"

class FUltraControlSubSystemEditorModule : public IModuleInterface
{
protected:
    TArray<TSharedPtr<FUCSS_AssetTypeActions_SubsystemBase>> ActionTypes;
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
