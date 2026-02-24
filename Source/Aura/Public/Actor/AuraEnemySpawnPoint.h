#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "AuraEnemySpawnPoint.generated.h"

UCLASS()
class AURA_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AAuraEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EnemyLevel = 1;
};
