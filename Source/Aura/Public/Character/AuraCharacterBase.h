#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	virtual FVector GetCombatSocketLocation() const override;

	virtual UAnimMontage* GetHitReactMontage_Implementation() const override;

	virtual void Die() override;

	//使客户端和服务器同时做表现层相关的死亡行为
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo();

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const;

	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category="Combat")
	FName WeaponSocketName;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	//初始化主要属性GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	//持续根据主要属性计算次要属性GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	//初始化重要属性GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> InitVitalAttributes;


	// Dissolve Effects
	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> CharacterDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

private:
	//初始就拥有的能力
	UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	//受击动画
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
