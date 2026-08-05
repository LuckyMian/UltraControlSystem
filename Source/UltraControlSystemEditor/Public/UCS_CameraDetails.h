#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class ULTRACONTROLSYSTEMEDITOR_API FUCS_CameraDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
