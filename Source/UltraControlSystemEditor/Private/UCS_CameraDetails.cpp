#include "UCS_CameraDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"

TSharedRef<IDetailCustomization> FUCS_CameraDetails::MakeInstance()
{
	return MakeShareable(new FUCS_CameraDetails);
}

void FUCS_CameraDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	DetailBuilder.EditCategory("UCS").SetSortOrder(0);
}
