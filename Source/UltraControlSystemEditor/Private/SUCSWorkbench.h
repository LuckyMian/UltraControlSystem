#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class AActor;
class IDetailsView;
template<typename ItemType> class SListView;
class SEditableTextBox;

struct FUCSWorkbenchItem
{
	TWeakObjectPtr<AActor> Actor;
	FString Type;
	FString KeyOrGroups;
	FString Relationship;
	int32 IssueCount = 0;
};

class SUCSWorkbench : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUCSWorkbench) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	void RefreshItems();
	void ApplyFilter();
	TSharedRef<ITableRow> GenerateRow(TSharedPtr<FUCSWorkbenchItem> Item, const TSharedRef<STableViewBase>& OwnerTable);
	void HandleSelectionChanged(TSharedPtr<FUCSWorkbenchItem> Item, ESelectInfo::Type SelectInfo);
	void HandleFilterChanged(const FText& NewText);
	FReply CreateCamera();
	FReply CreateWidget();
	FReply DuplicateSelected();
	FReply DeleteSelected();
	FReply FocusSelected();
	FReply ValidateScene();
	FReply ApplyCameraKey();
	bool HasSelection() const;
	AActor* GetSelectedActor() const;
	UWorld* GetEditorWorld() const;
	AActor* CreateActor(UClass* ActorClass, const FText& TransactionText, const FString& LabelPrefix);
	uint32 ComputeSceneSignature() const;
	EActiveTimerReturnType HandleActiveTimer(double CurrentTime, float DeltaTime);

	TArray<TSharedPtr<FUCSWorkbenchItem>> Items;
	TArray<TSharedPtr<FUCSWorkbenchItem>> FilteredItems;
	TSharedPtr<SListView<TSharedPtr<FUCSWorkbenchItem>>> ListView;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<SEditableTextBox> CameraKeyTextBox;
	TWeakObjectPtr<AActor> SelectedActor;
	FString Filter;
	uint32 LastSceneSignature = 0;
};
