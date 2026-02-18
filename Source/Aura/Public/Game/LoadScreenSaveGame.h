// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum class ESaveSlotStatus:uint8
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT(BlueprintType)
struct FActorSaveInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform Transform = FTransform();

	//包含被UPROPERTY(SaveGame)标记的序列化后的变量
	UPROPERTY()
	TArray<uint8> Bytes;
};

inline bool operator==(const FActorSaveInfo& A, const FActorSaveInfo& B)
{
	return A.ActorName == B.ActorName;
}

USTRUCT(BlueprintType)
struct FMapSaveInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName = FString();

	UPROPERTY()
	TArray<FActorSaveInfo> SavedActors;
};

USTRUCT(BlueprintType)
struct FAbilitySaveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ClassDefaults")
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatus = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityInputTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel;
};

inline bool operator==(const FAbilitySaveInfo& A, const FAbilitySaveInfo& B)
{
	return A.AbilityTag.MatchesTagExact(B.AbilityTag);
}

UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	//Slot

	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	FString PlayerName = FString("Default Name");

	UPROPERTY()
	FString MapName = FString("Default Map Name");

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	ESaveSlotStatus SaveSlotStatus = ESaveSlotStatus::Vacant;

	//Player

	UPROPERTY()
	int32 Level = 1;

	UPROPERTY()
	int32 XP = 0;

	UPROPERTY()
	int32 SpellPoint = 0;

	UPROPERTY()
	int32 AttributePoint = 0;

	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	UPROPERTY()
	float Strength = 0.f;

	UPROPERTY()
	float Intelligence = 0.f;

	UPROPERTY()
	float Resilience = 0.f;

	UPROPERTY()
	float Vigor = 0.f;

	UPROPERTY()
	TArray<FAbilitySaveInfo> SavedAbilities;

	UPROPERTY()
	TArray<FMapSaveInfo> SavedMaps;

	FMapSaveInfo GetMapSaveInfoByName(const FString& InMapName);

	bool HasMap(const FString& InMapName);
};
