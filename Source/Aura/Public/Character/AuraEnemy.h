#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UBehaviorTree;
class UWidgetComponent;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	//begin IHighlightInterface

	virtual void HighlightActor_Implementation() override;
	virtual void UnhighlightActor_Implementation() override;
	virtual void GetMoveToLocation_Implementation(FVector& OutLocation) override;
	//end IHighlightInterface

	//begin IEnemyInterface

	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	//end IEnemyInterface

	//begin ICombatInterface
	FORCEINLINE virtual int32 GetCharacterLevel_Implementation() const override { return Level; }

	virtual void Die(const FVector& DeathImpulse = FVector::ZeroVector) override;
	//end ICombatInterface

	//广播生命值变化
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag CallBackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category="Enemy|Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Combat")
	float LifeSpan = 5.f;

	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;

	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Character Class Default")
	int32 Level = 1;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBarComponent;

	UPROPERTY(EditAnywhere, Category="Enemy|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;

	UPROPERTY()
	TWeakObjectPtr<AActor> CombatTarget;

	virtual void OnStunTagChanged(const FGameplayTag CallBackTag, int32 NewCount) override;

private:
	void SetRenderCustomDepth(const bool bValue) const;
};
