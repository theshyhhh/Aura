#include "Actor/AuraEnemySpawnPoint.h"

#include "Character/AuraEnemy.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

void AAuraEnemySpawnPoint::SpawnEnemy() const
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(EnemyClass, GetActorTransform());
	Enemy->SetEnemyLevel(EnemyLevel);
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}
