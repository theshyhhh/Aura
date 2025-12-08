#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&/* Asset Tags */)
DECLARE_MULTICAST_DELEGATE(FOnAbilityGranted)
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)

UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	//在InitAbilityActorInfo后调用
	void AbilityActorInfoSet();

	//添加一开始就拥有的技能
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);

	FEffectAssetTags EffectAssetTags;

	//按下和持续按住触发的函数
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	//松开触发的函数
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	//能力被赋予时触发的委托
	FOnAbilityGranted OnAbilityGrantedDelegate;

	//初始技能是否已被赋予
	bool bStartupAbilityHasBeenGiven = false;

	void ForEachAbility(const FForEachAbility& Delegate);

	FGameplayTag GetTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayTag GetInputTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec);

	void UpgradeAttribute(FGameplayTag AttributeTag);

protected:
	UFUNCTION(Client, Reliable) //保证在客户端调用
	void ClientEffectAppliedToSelf(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec,
	                               FActiveGameplayEffectHandle ActiveEffectHandle) const;
	virtual void OnRep_ActivateAbilities() override;

private:
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(FGameplayTag AttributeTag);
};
