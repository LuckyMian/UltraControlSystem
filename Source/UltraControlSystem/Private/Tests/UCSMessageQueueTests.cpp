#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "UltraControlSystem/Support/UCS_PendingMessageQueue.h"

namespace
{
	FUCS_LogicStruct MakeLogic(const FString& Key)
	{
		FUCS_LogicStruct Logic;
		Logic.InfoMap.Add(TEXT("CnName"), Key);
		Logic.LogicMap.Add(TEXT("Key"), Key);
		return Logic;
	}

	FString DescribeMatches(const TArray<FUCS_QueuedLogicMessage>& Messages)
	{
		TArray<FString> Keys;
		Keys.Reserve(Messages.Num());
		for (const FUCS_QueuedLogicMessage& Message : Messages)
		{
			Keys.Add(Message.Key);
		}
		return FString::Join(Keys, TEXT(","));
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FUCSMessageQueuePendingStoreTest,
	"UltraControlSystem.MessageQueue.PendingStore",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FUCSMessageQueuePendingStoreTest::RunTest(const FString& Parameters)
{
	FUCS_PendingMessageQueue Queue;
	TestTrue(TEXT("A fresh queue is empty"), Queue.IsEmpty());

	// Messages without a Key can never be routed to a late receiver.
	FUCS_LogicStruct KeylessLogic;
	KeylessLogic.LogicMap.Add(TEXT("Key"), TEXT(""));
	TestEqual(TEXT("Keyless messages are rejected"), Queue.Enqueue(KeylessLogic, 0.0, 32), static_cast<int64>(0));
	TestEqual(TEXT("Keyless messages are not stored"), Queue.Num(), 0);

	const int64 FirstSequence = Queue.Enqueue(MakeLogic(TEXT("CameraA")), 1.0, 32);
	TestTrue(TEXT("Enqueue hands out a sequence"), FirstSequence > 0);
	TestEqual(TEXT("One state is stored"), Queue.Num(), 1);

	Queue.Enqueue(MakeLogic(TEXT("CameraB")), 2.0, 32);
	TestEqual(TEXT("Two states are stored"), Queue.Num(), 2);

	const FUCS_QueuedLogicMessage* Latest = Queue.GetLatest();
	TestNotNull(TEXT("Latest state exists"), Latest);
	if (Latest != nullptr)
	{
		TestEqual(TEXT("Latest state is the most recent key"), Latest->Key, FString(TEXT("CameraB")));
	}

	// Re-sending a key replaces its state instead of stacking another entry.
	const int64 OverwriteSequence = Queue.Enqueue(MakeLogic(TEXT("CameraA")), 3.0, 32);
	TestTrue(TEXT("Overwrite advances the sequence"), OverwriteSequence > FirstSequence);
	TestEqual(TEXT("Same key still stores a single state"), Queue.Num(), 2);

	Latest = Queue.GetLatest();
	TestNotNull(TEXT("Latest state exists after overwrite"), Latest);
	if (Latest != nullptr)
	{
		TestEqual(TEXT("Overwritten key becomes the latest state"), Latest->Key, FString(TEXT("CameraA")));
	}

	const FUCS_QueuedLogicMessage* Stored = Queue.FindByKey(TEXT("CameraA"));
	TestNotNull(TEXT("Stored key can be looked up"), Stored);
	if (Stored != nullptr)
	{
		TestEqual(TEXT("Overwrite keeps the newest sequence"), Stored->Sequence, OverwriteSequence);
		TestEqual(TEXT("Overwrite keeps the newest payload"), Stored->Logic.InfoMap.FindRef(TEXT("CnName")), FString(TEXT("CameraA")));
		TestEqual(TEXT("Overwrite keeps the newest timestamp"), Stored->QueuedTimeSeconds, 3.0);
	}
	TestNull(TEXT("Unknown keys are not stored"), Queue.FindByKey(TEXT("CameraZ")));

	TArray<FUCS_QueuedLogicMessage> Ordered = Queue.GetAll();
	TestEqual(TEXT("GetAll returns every retained state"), Ordered.Num(), 2);
	if (Ordered.Num() == 2)
	{
		TestTrue(TEXT("GetAll is ordered by sequence"), Ordered[0].Sequence < Ordered[1].Sequence);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FUCSMessageQueueTagMatchingTest,
	"UltraControlSystem.MessageQueue.TagMatching",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FUCSMessageQueueTagMatchingTest::RunTest(const FString& Parameters)
{
	FUCS_PendingMessageQueue Queue;
	Queue.Enqueue(MakeLogic(TEXT("CameraA")), 1.0, 32);
	Queue.Enqueue(MakeLogic(TEXT("WidgetB")), 2.0, 32);

	TArray<FName> CameraTags;
	CameraTags.Add(TEXT("CameraA"));
	CameraTags.Add(TEXT("Shared"));
	const TArray<FUCS_QueuedLogicMessage> CameraMatches = Queue.FindMatchingMessages(CameraTags);
	TestEqual(TEXT("Only the matching key is replayed"), DescribeMatches(CameraMatches), FString(TEXT("CameraA")));

	TArray<FName> NoTags;
	TestEqual(TEXT("Actors without tags match nothing"), Queue.FindMatchingMessages(NoTags).Num(), 0);

	TArray<FName> UnrelatedTags;
	UnrelatedTags.Add(TEXT("CameraC"));
	TestEqual(TEXT("Unrelated tags match nothing"), Queue.FindMatchingMessages(UnrelatedTags).Num(), 0);

	// Tag lookup goes through FName, exactly like AActor::ActorHasTag, so it follows
	// the name table's case-insensitive comparison instead of a raw string compare.
	TArray<FName> DifferentCaseTags;
	DifferentCaseTags.Add(TEXT("cameraa"));
	TestEqual(TEXT("Tag matching mirrors AActor::ActorHasTag"), DescribeMatches(Queue.FindMatchingMessages(DifferentCaseTags)), FString(TEXT("CameraA")));

	TArray<FName> MultiTags;
	MultiTags.Add(TEXT("WidgetB"));
	MultiTags.Add(TEXT("CameraA"));
	const TArray<FUCS_QueuedLogicMessage> MultiMatches = Queue.FindMatchingMessages(MultiTags);
	TestEqual(TEXT("Multiple tags match multiple keys"), DescribeMatches(MultiMatches), FString(TEXT("CameraA,WidgetB")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FUCSMessageQueueRetentionTest,
	"UltraControlSystem.MessageQueue.Retention",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FUCSMessageQueueRetentionTest::RunTest(const FString& Parameters)
{
	FUCS_PendingMessageQueue Queue;
	Queue.Enqueue(MakeLogic(TEXT("A")), 1.0, 2);
	Queue.Enqueue(MakeLogic(TEXT("B")), 2.0, 2);
	Queue.Enqueue(MakeLogic(TEXT("C")), 3.0, 2);

	TestEqual(TEXT("The cap is enforced"), Queue.Num(), 2);
	TestFalse(TEXT("The oldest state is evicted"), Queue.ContainsKey(TEXT("A")));
	TestTrue(TEXT("Newer states survive"), Queue.ContainsKey(TEXT("B")));
	TestTrue(TEXT("Newer states survive"), Queue.ContainsKey(TEXT("C")));

	if (const FUCS_QueuedLogicMessage* Latest = Queue.GetLatest())
	{
		TestEqual(TEXT("Latest still points at the newest key"), Latest->Key, FString(TEXT("C")));
	}
	else
	{
		AddError(TEXT("Latest state exists after eviction"));
	}

	Queue.Clear();
	TestTrue(TEXT("Clear empties the queue"), Queue.IsEmpty());
	TestEqual(TEXT("Clear resets sequences"), Queue.GetLastSequence(), static_cast<int64>(0));
	TestNull(TEXT("Clear removes stored states"), Queue.FindByKey(TEXT("C")));

	const int64 RestartSequence = Queue.Enqueue(MakeLogic(TEXT("D")), 4.0, 2);
	TestTrue(TEXT("The queue is reusable after Clear"), RestartSequence > 0);
	TestEqual(TEXT("Reused queue stores the new state"), Queue.Num(), 1);

	return true;
}

#endif
