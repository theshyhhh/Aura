#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	//begin IEnemyInterface
	virtual void HighlightActor() override;

	virtual void UnHighlightActor() override;
	//end IEnemyInterface

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted;

	virtual void BeginPlay() override;

private:
	void SetRenderCustomDepth(const bool bValue);
};
