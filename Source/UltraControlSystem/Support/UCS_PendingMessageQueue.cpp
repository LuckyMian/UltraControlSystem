// Fill out your copyright notice in the Description page of Project Settings.

#include "UCS_PendingMessageQueue.h"

int64 FUCS_PendingMessageQueue::Enqueue(const FUCS_LogicStruct& LogicInput, double NowSeconds, int32 MaxKeys)
{
	const FString* KeyPtr = LogicInput.LogicMap.Find(TEXT("Key"));
	if (KeyPtr == nullptr || KeyPtr->IsEmpty())
	{
		return 0;
	}

	const int64 Sequence = NextSequence++;

	FUCS_QueuedLogicMessage Message;
	Message.Logic = LogicInput;
	Message.Key = *KeyPtr;
	Message.Sequence = Sequence;
	Message.QueuedTimeSeconds = NowSeconds;

	MessagesByKey.Add(Message.Key, MoveTemp(Message));
	EvictOldestIfNeeded(MaxKeys);
	return Sequence;
}

const FUCS_QueuedLogicMessage* FUCS_PendingMessageQueue::FindByKey(const FString& Key) const
{
	return MessagesByKey.Find(Key);
}

const FUCS_QueuedLogicMessage* FUCS_PendingMessageQueue::GetLatest() const
{
	const FUCS_QueuedLogicMessage* Latest = nullptr;
	for (const TPair<FString, FUCS_QueuedLogicMessage>& Pair : MessagesByKey)
	{
		if (Latest == nullptr || Pair.Value.Sequence > Latest->Sequence)
		{
			Latest = &Pair.Value;
		}
	}
	return Latest;
}

TArray<FUCS_QueuedLogicMessage> FUCS_PendingMessageQueue::GetAll() const
{
	TArray<FUCS_QueuedLogicMessage> Result;
	Result.Reserve(MessagesByKey.Num());
	for (const TPair<FString, FUCS_QueuedLogicMessage>& Pair : MessagesByKey)
	{
		Result.Add(Pair.Value);
	}

	Result.Sort([](const FUCS_QueuedLogicMessage& Left, const FUCS_QueuedLogicMessage& Right)
	{
		return Left.Sequence < Right.Sequence;
	});
	return Result;
}

TArray<FUCS_QueuedLogicMessage> FUCS_PendingMessageQueue::FindMatchingMessages(const TArray<FName>& Tags) const
{
	TArray<FUCS_QueuedLogicMessage> Result;
	if (Tags.Num() == 0)
	{
		return Result;
	}

	for (const TPair<FString, FUCS_QueuedLogicMessage>& Pair : MessagesByKey)
	{
		// FName comparison keeps this identical to AActor::ActorHasTag, which the
		// live dispatch path uses for cameras and scene widgets.
		if (Tags.Contains(FName(*Pair.Key)))
		{
			Result.Add(Pair.Value);
		}
	}

	Result.Sort([](const FUCS_QueuedLogicMessage& Left, const FUCS_QueuedLogicMessage& Right)
	{
		return Left.Sequence < Right.Sequence;
	});
	return Result;
}

bool FUCS_PendingMessageQueue::ContainsKey(const FString& Key) const
{
	return MessagesByKey.Contains(Key);
}

void FUCS_PendingMessageQueue::Clear()
{
	MessagesByKey.Reset();
	NextSequence = 1;
}

void FUCS_PendingMessageQueue::EvictOldestIfNeeded(int32 MaxKeys)
{
	if (MaxKeys <= 0)
	{
		return;
	}

	while (MessagesByKey.Num() > MaxKeys)
	{
		const FString* OldestKey = nullptr;
		int64 OldestSequence = 0;
		for (const TPair<FString, FUCS_QueuedLogicMessage>& Pair : MessagesByKey)
		{
			if (OldestKey == nullptr || Pair.Value.Sequence < OldestSequence)
			{
				OldestKey = &Pair.Key;
				OldestSequence = Pair.Value.Sequence;
			}
		}

		if (OldestKey == nullptr)
		{
			return;
		}

		MessagesByKey.Remove(*OldestKey);
	}
}
