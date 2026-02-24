#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class AAuraEnemySpawnPoint;
class UBoxComponent;

UCLASS()
class AURA_API AAuraEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()

public:
	AAuraEnemySpawnVolume();

	UPROPERTY(SaveGame, BlueprintReadWrite)
	bool bReached = false;

	//SaveInterface Begin

	virtual void LoadActor_Implementation() override;

	virtual bool ShouldSetTransform_Implementation() override;

	//SaveInterface End

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                               int32 OtherBodyIndex,
	                               bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere)
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;
};
