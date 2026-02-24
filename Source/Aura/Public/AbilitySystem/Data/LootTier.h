#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTier.generated.h"

USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LootItem")
	TSubclassOf<AActor> LootClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LootItem")
	float SpawnChance = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LootItem")
	int32 MaxSpawnNum = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LootItem")
	bool bOverrideLootLevel = true;
};

UCLASS()
class AURA_API ULootTier : public UDataAsset
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,BlueprintPure)
	TArray<FLootItem> GetRandomLoot();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LootTier")
	TArray<FLootItem> LootItems;
};
