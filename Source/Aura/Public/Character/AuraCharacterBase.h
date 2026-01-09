#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UDebuffNiagaraComponent;
class UNiagaraSystem;
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
	//CombatInterface Begin
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& Tag) const override;

	virtual UAnimMontage* GetHitReactMontage_Implementation() const override;

	virtual void Die(const FVector& DeathImpulse = FVector::ZeroVector) override;

	virtual bool IsDead_Implementation() const override;

	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override;

	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;

	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& Tag) const override;

	virtual int32 GetMinionCount_Implementation() override;

	virtual void IncreaseMinionCount_Implementation(int32 Amount) override;

	virtual ECharacterClass GetCharacterClass_Implementation() override;

	virtual FOnASCRegisteredSignature& GetOnASCRegisteredDelegate() override;

	virtual FOnDeathSignature& GetOnDeathDelegate() override;

	virtual USkeletalMeshComponent* GetWeaponMesh_Implementation() override;
	//CombatInterface End

	//使客户端和服务器同时做表现层相关的死亡行为
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse = FVector::ZeroVector);

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo();

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const;

	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AuraCharacter|Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category="AuraCharacter|Combat|Socket")
	FName WeaponSocketName;

	UPROPERTY(EditAnywhere, Category="AuraCharacter|Combat|Socket")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category="AuraCharacter|Combat|Socket")
	FName RightHandSocketName;

	UPROPERTY(EditAnywhere, Category="AuraCharacter|Combat|Socket")
	FName TailSocketName;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	//初始化主要属性GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AuraCharacter|Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	//持续根据主要属性计算次要属性GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AuraCharacter|Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	//初始化重要属性GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AuraCharacter|Attributes")
	TSubclassOf<UGameplayEffect> InitVitalAttributes;


	// Dissolve Effects
	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartCharacterDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AuraCharacter|Combat")
	TObjectPtr<UMaterialInstance> CharacterDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AuraCharacter|Combat")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	bool bDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AuraCharacter|Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;

	//召唤物数量
	int32 MinionCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy|Character Class Default")
	ECharacterClass CharacterClass = ECharacterClass::Elementalist;

	FOnASCRegisteredSignature OnASCRegisteredDelegate;

	FOnDeathSignature OnDeathDelegate;

private:
	//初始就拥有的能力
	UPROPERTY(EditAnywhere, Category="AuraCharacter|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category="AuraCharacter|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;

	//受击动画
	UPROPERTY(EditDefaultsOnly, Category="AuraCharacter|Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	//攻击动画和对应的Tag
	UPROPERTY(EditDefaultsOnly, Category="AuraCharacter|Combat")
	TArray<FTaggedMontage> AttackMontages;

	UPROPERTY(VisibleAnywhere, Category="AuraCharacter|Combat")
	TObjectPtr<UDebuffNiagaraComponent> BurnNiagaraComponent;
};
