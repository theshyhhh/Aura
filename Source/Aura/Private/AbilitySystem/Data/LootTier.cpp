#include "AbilitySystem/Data/LootTier.h"

TArray<FLootItem> ULootTier::GetRandomLoot()
{
	TArray<FLootItem> result;
	for (const FLootItem& Item : LootItems)
	{
		for (int32 i = 0; i < Item.MaxSpawnNum; i++)
		{
			if (FMath::RandRange(0.f, 100.f) < Item.SpawnChance)
			{
				FLootItem NewItem = {Item.LootClass, 0, 0, Item.bOverrideLootLevel};
				result.Add(NewItem);
			}
		}
	}
	return result;
}
