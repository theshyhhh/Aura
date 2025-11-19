// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RequiredXP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 AttributePointsReward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 SpellPointReward;
};

UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LevelUpInfomation")
	TArray<FAuraLevelUpInfo> LevelUpInfo;

	int32 FindLevelByXP(const int32 XP) const;
};
