#include "SUCSWorkbench.h"

#include "../../UltraControlSystem/Actor/UCS_Camera.h"
#include "../../UltraControlSystem/Actor/UCS_Widget.h"
#include "DetailLayoutBuilder.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "Engine/Selection.h"
#include "Framework/Application/SlateApplication.h"
#include "IDetailsView.h"
#include "LevelEditorViewport.h"
#include "Misc/MessageDialog.h"
#include "PropertyEditorModule.h"
#include "ScopedTransaction.h"
#include "UCS_EditorValidation.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"

#define LOCTEXT_NAMESPACE "SUCSWorkbench"

void SUCSWorkbench::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropertyEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	FDetailsViewArgs DetailsArgs;
	DetailsArgs.bAllowSearch = true;
	DetailsArgs.bHideSelectionTip = true;
	DetailsArgs.NotifyHook = nullptr;
	DetailsView = PropertyEditor.CreateDetailView(DetailsArgs);

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().Padding(4)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().Padding(2)[SNew(SButton).Text(LOCTEXT("Refresh", "Refresh")).OnClicked_Lambda([this]() { RefreshItems(); return FReply::Handled(); })]
			+ SHorizontalBox::Slot().AutoWidth().Padding(2)[SNew(SButton).Text(LOCTEXT("AddCamera", "+ Camera")).OnClicked(this, &SUCSWorkbench::CreateCamera)]
			+ SHorizontalBox::Slot().AutoWidth().Padding(2)[SNew(SButton).Text(LOCTEXT("AddWidget", "+ Widget")).OnClicked(this, &SUCSWorkbench::CreateWidget)]
			+ SHorizontalBox::Slot().AutoWidth().Padding(2)[SNew(SButton).Text(LOCTEXT("Duplicate", "Duplicate")).IsEnabled(this, &SUCSWorkbench::HasSelection).OnClicked(this, &SUCSWorkbench::DuplicateSelected)]
			+ SHorizontalBox::Slot().AutoWidth().Padding(2)[SNew(SButton).Text(LOCTEXT("Delete", "Delete")).IsEnabled(this, &SUCSWorkbench::HasSelection).OnClicked(this, &SUCSWorkbench::DeleteSelected)]
			+ SHorizontalBox::Slot().AutoWidth().Padding(2)[SNew(SButton).Text(LOCTEXT("Focus", "Focus")).IsEnabled(this, &SUCSWorkbench::HasSelection).OnClicked(this, &SUCSWorkbench::FocusSelected)]
			+ SHorizontalBox::Slot().AutoWidth().Padding(2)[SNew(SButton).Text(LOCTEXT("Validate", "Validate")).OnClicked(this, &SUCSWorkbench::ValidateScene)]
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(6, 2)
		[
			SNew(SSearchBox).HintText(LOCTEXT("SearchHint", "Search label, Key, Group or type...")).OnTextChanged(this, &SUCSWorkbench::HandleFilterChanged)
		]
		+ SVerticalBox::Slot().FillHeight(1.0f).Padding(4)
		[
			SNew(SSplitter)
			+ SSplitter::Slot().Value(0.46f)
			[
				SAssignNew(ListView, SListView<TSharedPtr<FUCSWorkbenchItem>>)
				.ListItemsSource(&FilteredItems)
				.OnGenerateRow(this, &SUCSWorkbench::GenerateRow)
				.OnSelectionChanged(this, &SUCSWorkbench::HandleSelectionChanged)
			]
			+ SSplitter::Slot().Value(0.54f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().Padding(4)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().FillWidth(1.0f)
					[
						SAssignNew(CameraKeyTextBox, SEditableTextBox).HintText(LOCTEXT("KeyHint", "Selected Camera LogicMap.Key"))
					]
					+ SHorizontalBox::Slot().AutoWidth().Padding(4, 0)
					[
						SNew(SButton).Text(LOCTEXT("ApplyKey", "Apply Key + Sync Groups")).OnClicked(this, &SUCSWorkbench::ApplyCameraKey)
					]
				]
				+ SVerticalBox::Slot().FillHeight(1.0f)[DetailsView.ToSharedRef()]
			]
		]
	];

	RefreshItems();
	RegisterActiveTimer(1.0f, FWidgetActiveTimerDelegate::CreateSP(this, &SUCSWorkbench::HandleActiveTimer));
}

UWorld* SUCSWorkbench::GetEditorWorld() const
{
	return GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
}

void SUCSWorkbench::RefreshItems()
{
	const FString SelectedPath = SelectedActor.IsValid() ? SelectedActor->GetPathName() : FString();
	Items.Reset();
	UWorld* World = GetEditorWorld();
	if (!IsValid(World))
	{
		ApplyFilter();
		return;
	}
	LastSceneSignature = ComputeSceneSignature();

	const TArray<FUCSValidationIssue> Issues = FUCSSceneValidator::Validate(World);
	TMap<TWeakObjectPtr<AActor>, int32> IssueCounts;
	for (const FUCSValidationIssue& Issue : Issues)
	{
		if (Issue.Actor.IsValid()) ++IssueCounts.FindOrAdd(Issue.Actor);
	}

	TMap<FString, int32> WidgetLinks;
	for (TActorIterator<AUCS_Widget> It(World); It; ++It)
	{
		for (const FName Group : It->Groups) ++WidgetLinks.FindOrAdd(Group.ToString().ToLower());
	}

	for (TActorIterator<AUCS_Camera> It(World); It; ++It)
	{
		TSharedPtr<FUCSWorkbenchItem> Item = MakeShared<FUCSWorkbenchItem>();
		Item->Actor = *It;
		Item->Type = TEXT("Camera");
		Item->KeyOrGroups = It->Info.LogicMap.FindRef(TEXT("Key"));
		Item->Relationship = FString::Printf(TEXT("%d linked widget(s)"), WidgetLinks.FindRef(Item->KeyOrGroups.ToLower()));
		Item->IssueCount = IssueCounts.FindRef(*It);
		Items.Add(Item);
	}

	for (TActorIterator<AUCS_Widget> It(World); It; ++It)
	{
		TSharedPtr<FUCSWorkbenchItem> Item = MakeShared<FUCSWorkbenchItem>();
		Item->Actor = *It;
		Item->Type = TEXT("Widget");
		TArray<FString> Groups;
		for (const FName Group : It->Groups) Groups.Add(Group.ToString());
		Item->KeyOrGroups = FString::Join(Groups, TEXT(", "));
		Item->Relationship = FString::Printf(TEXT("%d group(s)"), It->Groups.Num());
		Item->IssueCount = IssueCounts.FindRef(*It);
		Items.Add(Item);
	}

	Items.Sort([](const TSharedPtr<FUCSWorkbenchItem>& A, const TSharedPtr<FUCSWorkbenchItem>& B)
	{
		if (A->Type != B->Type) return A->Type < B->Type;
		return A->Actor->GetActorLabel() < B->Actor->GetActorLabel();
	});
	ApplyFilter();

	if (!SelectedPath.IsEmpty())
	{
		for (const TSharedPtr<FUCSWorkbenchItem>& Item : FilteredItems)
		{
			if (Item->Actor.IsValid() && Item->Actor->GetPathName() == SelectedPath)
			{
				ListView->SetSelection(Item);
				break;
			}
		}
	}
}

uint32 SUCSWorkbench::ComputeSceneSignature() const
{
	uint32 Signature = 0;
	UWorld* World = GetEditorWorld();
	if (!IsValid(World)) return Signature;
	for (TActorIterator<AUCS_Camera> It(World); It; ++It)
	{
		Signature = HashCombineFast(Signature, GetTypeHash(It->GetPathName()));
		Signature = HashCombineFast(Signature, GetTypeHash(It->GetActorLabel()));
		Signature = HashCombineFast(Signature, GetTypeHash(It->Info.LogicMap.FindRef(TEXT("Key"))));
		Signature = HashCombineFast(Signature, GetTypeHash(It->ClampPitch));
		Signature = HashCombineFast(Signature, GetTypeHash(It->ClampYaw));
	}
	for (TActorIterator<AUCS_Widget> It(World); It; ++It)
	{
		Signature = HashCombineFast(Signature, GetTypeHash(It->GetPathName()));
		Signature = HashCombineFast(Signature, GetTypeHash(It->GetActorLabel()));
		Signature = HashCombineFast(Signature, GetTypeHash(It->WidgetClass));
		for (const FName Group : It->Groups) Signature = HashCombineFast(Signature, GetTypeHash(Group));
	}
	return Signature;
}

EActiveTimerReturnType SUCSWorkbench::HandleActiveTimer(double CurrentTime, float DeltaTime)
{
	if (ComputeSceneSignature() != LastSceneSignature) RefreshItems();
	return EActiveTimerReturnType::Continue;
}

void SUCSWorkbench::ApplyFilter()
{
	FilteredItems.Reset();
	for (const TSharedPtr<FUCSWorkbenchItem>& Item : Items)
	{
		const FString Haystack = FString::Printf(TEXT("%s %s %s %s"), *Item->Type,
			Item->Actor.IsValid() ? *Item->Actor->GetActorLabel() : TEXT(""), *Item->KeyOrGroups, *Item->Relationship);
		if (Filter.IsEmpty() || Haystack.Contains(Filter, ESearchCase::IgnoreCase)) FilteredItems.Add(Item);
	}
	if (ListView.IsValid()) ListView->RequestListRefresh();
}

TSharedRef<ITableRow> SUCSWorkbench::GenerateRow(TSharedPtr<FUCSWorkbenchItem> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	const FString Label = Item->Actor.IsValid() ? Item->Actor->GetActorLabel() : TEXT("<deleted>");
	const FString Issues = Item->IssueCount > 0 ? FString::Printf(TEXT("  ! %d"), Item->IssueCount) : FString();
	return SNew(STableRow<TSharedPtr<FUCSWorkbenchItem>>, OwnerTable)
	[
		SNew(SBorder).Padding(6)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("[%s] %s%s"), *Item->Type, *Label, *Issues)))]
			+ SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text(FText::FromString(Item->KeyOrGroups)).ColorAndOpacity(FSlateColor::UseSubduedForeground())]
			+ SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text(FText::FromString(Item->Relationship)).ColorAndOpacity(FSlateColor::UseSubduedForeground())]
		]
	];
}

void SUCSWorkbench::HandleSelectionChanged(TSharedPtr<FUCSWorkbenchItem> Item, ESelectInfo::Type SelectInfo)
{
	SelectedActor = Item.IsValid() ? Item->Actor : nullptr;
	DetailsView->SetObject(SelectedActor.Get());
	if (AUCS_Camera* Camera = Cast<AUCS_Camera>(SelectedActor.Get()))
	{
		CameraKeyTextBox->SetText(FText::FromString(Camera->Info.LogicMap.FindRef(TEXT("Key"))));
	}
	else
	{
		CameraKeyTextBox->SetText(FText::GetEmpty());
	}
}

void SUCSWorkbench::HandleFilterChanged(const FText& NewText)
{
	Filter = NewText.ToString();
	ApplyFilter();
}

AActor* SUCSWorkbench::CreateActor(UClass* ActorClass, const FText& TransactionText, const FString& LabelPrefix)
{
	UWorld* World = GetEditorWorld();
	if (!GEditor || !World || !World->GetCurrentLevel() || !ActorClass) return nullptr;

	FTransform Transform;
	if (const FLevelEditorViewportClient* Viewport = GCurrentLevelEditingViewportClient)
	{
		const FVector Forward = Viewport->GetViewRotation().Vector();
		Transform.SetLocation(Viewport->GetViewLocation() + Forward * 1000.0);
		Transform.SetRotation(Viewport->GetViewRotation().Quaternion());
	}

	const FScopedTransaction Transaction(TransactionText);
	AActor* Actor = GEditor->AddActor(World->GetCurrentLevel(), ActorClass, Transform, true, RF_Transactional, true);
	if (Actor)
	{
		Actor->Modify();
		FActorLabelUtilities::SetActorLabelUnique(Actor, LabelPrefix);
		Actor->MarkPackageDirty();
		GEditor->SelectNone(false, true);
		GEditor->SelectActor(Actor, true, true);
		SelectedActor = Actor;
		RefreshItems();
	}
	return Actor;
}

FReply SUCSWorkbench::CreateCamera()
{
	CreateActor(AUCS_Camera::StaticClass(), LOCTEXT("CreateCameraTransaction", "Create UCS Camera"), TEXT("UCS_Camera"));
	return FReply::Handled();
}

FReply SUCSWorkbench::CreateWidget()
{
	CreateActor(AUCS_Widget::StaticClass(), LOCTEXT("CreateWidgetTransaction", "Create UCS Widget"), TEXT("UCS_Widget"));
	return FReply::Handled();
}

bool SUCSWorkbench::HasSelection() const { return SelectedActor.IsValid(); }
AActor* SUCSWorkbench::GetSelectedActor() const { return SelectedActor.Get(); }

FReply SUCSWorkbench::DuplicateSelected()
{
	AActor* Actor = GetSelectedActor();
	if (!GEditor || !Actor || !Actor->GetLevel()) return FReply::Handled();
	GEditor->SelectNone(false, true);
	GEditor->SelectActor(Actor, true, true);
	GEditor->edactDuplicateSelected(Actor->GetLevel(), true);
	AActor* Duplicate = GEditor->GetSelectedActors()->GetTop<AActor>();
	if (AUCS_Camera* DuplicateCamera = Cast<AUCS_Camera>(Duplicate))
	{
		const FString BaseKey = CastChecked<AUCS_Camera>(Actor)->Info.LogicMap.FindRef(TEXT("Key")) + TEXT("_Copy");
		FString Candidate = BaseKey;
		int32 Suffix = 2;
		for (;;)
		{
			bool bExists = false;
			for (TActorIterator<AUCS_Camera> It(GetEditorWorld()); It; ++It)
			{
				if (*It != DuplicateCamera && It->Info.LogicMap.FindRef(TEXT("Key")).Equals(Candidate, ESearchCase::IgnoreCase))
				{
					bExists = true;
					Candidate = FString::Printf(TEXT("%s_%d"), *BaseKey, Suffix++);
					break;
				}
			}
			if (!bExists) break;
		}
		DuplicateCamera->Modify();
		DuplicateCamera->Info.LogicMap.Add(TEXT("Key"), Candidate);
		DuplicateCamera->PostEditChange();
		DuplicateCamera->MarkPackageDirty();
	}
	SelectedActor = Duplicate;
	RefreshItems();
	return FReply::Handled();
}

FReply SUCSWorkbench::DeleteSelected()
{
	AActor* Actor = GetSelectedActor();
	UWorld* World = GetEditorWorld();
	if (!GEditor || !Actor || !World) return FReply::Handled();
	if (FMessageDialog::Open(EAppMsgType::YesNo,
		FText::Format(LOCTEXT("DeleteConfirm", "Delete '{0}' from the current level? This can be undone."), FText::FromString(Actor->GetActorLabel()))) != EAppReturnType::Yes)
	{
		return FReply::Handled();
	}
	GEditor->SelectNone(false, true);
	GEditor->SelectActor(Actor, true, true);
	GEditor->edactDeleteSelected(World, false, true, true);
	SelectedActor.Reset();
	DetailsView->SetObject(nullptr);
	RefreshItems();
	return FReply::Handled();
}

FReply SUCSWorkbench::FocusSelected()
{
	if (AActor* Actor = GetSelectedActor())
	{
		GEditor->SelectNone(false, true);
		GEditor->SelectActor(Actor, true, true);
		GEditor->MoveViewportCamerasToActor(*Actor, false);
	}
	return FReply::Handled();
}

FReply SUCSWorkbench::ValidateScene()
{
	FUCSSceneValidator::PublishToMessageLog(FUCSSceneValidator::Validate(GetEditorWorld()), true);
	RefreshItems();
	return FReply::Handled();
}

FReply SUCSWorkbench::ApplyCameraKey()
{
	AUCS_Camera* Camera = Cast<AUCS_Camera>(GetSelectedActor());
	UWorld* World = GetEditorWorld();
	if (!Camera || !World) return FReply::Handled();

	FString NewKey = CameraKeyTextBox->GetText().ToString().TrimStartAndEnd();
	if (NewKey.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("KeyRequired", "镜头 Key 不能为空。"));
		return FReply::Handled();
	}
	for (TActorIterator<AUCS_Camera> It(World); It; ++It)
	{
		if (*It != Camera && It->Info.LogicMap.FindRef(TEXT("Key")).Equals(NewKey, ESearchCase::IgnoreCase))
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("KeyDuplicate", "其他镜头已经使用了这个 Key。"));
			return FReply::Handled();
		}
	}

	const FString OldKey = Camera->Info.LogicMap.FindRef(TEXT("Key"));
	TArray<AUCS_Widget*> LinkedWidgets;
	for (TActorIterator<AUCS_Widget> It(World); It; ++It)
	{
		if (It->Groups.ContainsByPredicate([&OldKey](FName Group) { return Group.ToString().Equals(OldKey, ESearchCase::IgnoreCase); })) LinkedWidgets.Add(*It);
	}
	if (!OldKey.Equals(NewKey, ESearchCase::CaseSensitive) && LinkedWidgets.Num() > 0)
	{
		const FText Prompt = FText::Format(LOCTEXT("SyncPrompt", "Rename Key '{0}' to '{1}' and update {2} linked scene Widget(s)?"),
			FText::FromString(OldKey), FText::FromString(NewKey), FText::AsNumber(LinkedWidgets.Num()));
		if (FMessageDialog::Open(EAppMsgType::YesNo, Prompt) != EAppReturnType::Yes) return FReply::Handled();
	}

	const FScopedTransaction Transaction(LOCTEXT("RenameKeyTransaction", "Rename UCS Camera Key"));
	Camera->Modify();
	Camera->Info.LogicMap.Add(TEXT("Key"), NewKey);
	for (FName& ActorTag : Camera->Tags) if (ActorTag.ToString().Equals(OldKey, ESearchCase::IgnoreCase)) ActorTag = FName(*NewKey);
	Camera->PostEditChange();
	Camera->MarkPackageDirty();
	for (AUCS_Widget* Widget : LinkedWidgets)
	{
		Widget->Modify();
		for (FName& Group : Widget->Groups) if (Group.ToString().Equals(OldKey, ESearchCase::IgnoreCase)) Group = FName(*NewKey);
		Widget->PostEditChange();
		Widget->MarkPackageDirty();
	}
	RefreshItems();
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
