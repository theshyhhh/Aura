#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	UFUNCTION(BlueprintPure, Category="Summon")
	TSubclassOf<APawn> GetRandomMinionClass();

	UPROPERTY(EditDefaultsOnly, Category="Summon")
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category="Summon")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category="Summon")
	float MinSpawnDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Summon")
	float MaxSpawnDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, Category="Summon")
	float SpawnSpread = 90.f;
};
