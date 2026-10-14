// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UCS_LogicStruct.h"
#include "UCS_PendingMessageQueue.generated.h"

/** One UCS logic message retained by the system so late receivers can catch up. */
USTRUCT(BlueprintType)
struct FUCS_QueuedLogicMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "UCS|Message Queue")
	FUCS_LogicStruct Logic;

	/** Copy of Logic.LogicMap["Key"]; messages without a Key are never queued. */
	UPROPERTY(BlueprintReadOnly, Category = "UCS|Message Queue")
	FString Key;

	/** Monotonic id; the highest value is the currently active state. */
	UPROPERTY(BlueprintReadOnly, Category = "UCS|Message Queue")
	int64 Sequence = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UCS|Message Queue")
	double QueuedTimeSeconds = 0.0;
};

/**
 * Last-wins store of active UCS logic states, keyed by FUCS_LogicStruct::LogicMap["Key"].
 *
 * Pure data and logic with no UObject or World dependency, so the catch-up rules can be
 * covered by automation tests. The owning subsystem decides when to enqueue, when to
 * evict, and how a matched message is delivered to a receiver.
 */
struct ULTRACONTROLSYSTEM_API FUCS_PendingMessageQueue
{
public:
	/**
	 * Stores Logic under its Key, replacing any previous state for that Key.
	 * Returns the assigned Sequence, or 0 when the message has no usable Key.
	 */
	int64 Enqueue(const FUCS_LogicStruct& LogicInput, double NowSeconds, int32 MaxKeys);

	const FUCS_QueuedLogicMessage* FindByKey(const FString& Key) const;

	/** Highest Sequence across all keys: the state that is currently active. */
	const FUCS_QueuedLogicMessage* GetLatest() const;

	/** All retained states ordered by Sequence. */
	TArray<FUCS_QueuedLogicMessage> GetAll() const;

	/**
	 * Messages whose Key matches one of Tags, ordered by Sequence.
	 * Matching goes through FName so it mirrors AActor::ActorHasTag exactly.
	 */
	TArray<FUCS_QueuedLogicMessage> FindMatchingMessages(const TArray<FName>& Tags) const;

	bool ContainsKey(const FString& Key) const;

	bool IsEmpty() const { return MessagesByKey.Num() == 0; }

	int32 Num() const { return MessagesByKey.Num(); }

	void Clear();

	/** Largest Sequence handed out so far; 0 when nothing was ever queued. */
	int64 GetLastSequence() const { return NextSequence - 1; }

private:
	void EvictOldestIfNeeded(int32 MaxKeys);

	TMap<FString, FUCS_QueuedLogicMessage> MessagesByKey;
	int64 NextSequence = 1;
};
