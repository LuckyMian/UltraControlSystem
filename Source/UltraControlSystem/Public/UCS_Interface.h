
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Support/UCS_LogicStruct.h"
#include "UCS_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UUCS_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ULTRACONTROLSYSTEM_API IUCS_Interface
{
	GENERATED_BODY()

public:
	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCS_Interface")
	bool MoveInterface(FVector2D MoveInput); 
	virtual bool MoveInterface_Implementation(FVector2D MoveInput);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCS_Interface")
	bool RotationInterface(FVector2D MoveInput); 
	virtual bool RotationInterface_Implementation(FVector2D MoveInput);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCS_Interface")
	bool ZoomInterface(float ZoomInput); 
	virtual bool ZoomInterface_Implementation(float ZoomInput);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCS_Interface")
	bool FocusPoiInterface(AActor* UCS_Actor); 
	virtual bool FocusPoiInterface_Implementation(AActor* UCS_Actor);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCS_Interface")
	bool LogicInterface(FUCS_LogicStruct LogicInput); 
	virtual bool LogicInterface_Implementation(FUCS_LogicStruct LogicInput);
	
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UCS_Interface")
	bool ShowWidget(FUCS_LogicStruct LogicInput); 
	virtual bool ShowWidget_Implementation(FUCS_LogicStruct LogicInput);

	virtual bool ShowPage(FUCS_LogicStruct LogicInput);
};
