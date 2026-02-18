#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class ULoadScreenSaveGame;
class UAuraAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&/* Asset Tags */)
DECLARE_MULTICAST_DELEGATE(FOnAbilityGranted)
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAbilityStatusChangedSignature, FGameplayTag/*Ability Tag*/, FGameplayTag/*Status Tag*/, int32 AbilityLevel)
DECLARE_MULTICAST_DELEGATE_FourParams(FOnAbilityEquippedSignature, const FGameplayTag&/*Ability Tag*/, const FGameplayTag&/*Status Tag*/,
                                      const FGameplayTag&/*NewInputTag*/, const FGameplayTag&/*PreviousInputTag*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPassiveAbilityDeactivatedSignature, const FGameplayTag&/*Ability Tag*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPassiveEffectActivatedSignature, const FGameplayTag&/*Ability Tag*/, bool/*bActivated*/)

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

	void AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData);

	//广播吃到药瓶时的GE信息
	FEffectAssetTags EffectAssetTags;

	//按下触发的函数

	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	//按下和持续按住触发的函数
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	//松开触发的函数
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	//能力被赋予时触发的委托
	FOnAbilityGranted OnAbilityGrantedDelegate;

	//技能状态改变时触发的委托
	FOnAbilityStatusChangedSignature OnAbilityStatusChangedDelegate;

	//初始技能是否已被赋予
	bool bStartupAbilityHasBeenGiven = false;

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec);

	static FGameplayTag GetInputTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec);

	static FGameplayTag GetStatusTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec);

	bool IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const;

	FGameplayAbilitySpec* GetAbilitySpecByTag(FGameplayTag AbilityTag);

	FGameplayTag GetStatusTagByAbilityTag(const FGameplayTag& AbilityTag);

	FGameplayTag GetInputTagByAbilityTag(const FGameplayTag& AbilityTag);

	FGameplayAbilitySpec* GetAbilitySpecByInputTag(const FGameplayTag& InputTag);

	bool IsInputSlotEmpty(const FGameplayTag& InputTag);

	void UpgradeAttribute(FGameplayTag AttributeTag);

	void UpdateAbilityStatus(int32 Level);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	bool GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag,
	                        const FGameplayTag& PrevInputTag);
	//技能装备时广播技能的各个标签信息
	FOnAbilityEquippedSignature OnAbilityEquippedDelegate;

	//取消被动技能时广播
	FOnPassiveAbilityDeactivatedSignature OnPassiveAbilityDeactivatedDelegate;

	FOnPassiveEffectActivatedSignature OnPassiveEffectActivatedDelegate;

protected:
	UFUNCTION(Client, Reliable) //保证在客户端调用
	void ClientEffectAppliedToSelf(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec,
	                               FActiveGameplayEffectHandle ActiveEffectHandle) const;
	virtual void OnRep_ActivateAbilities() override;

private:
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(FGameplayTag AttributeTag);

	UFUNCTION(Client, Reliable) //保证在客户端调用
	void ClientUpdateAbilityStatus(FGameplayTag AbilityTag, FGameplayTag StatusTag, int32 AbilityLevel);

	static void ClearAbilitySpecInputTags(FGameplayAbilitySpec* Spec);

	void ClearInputTagByInputTag(const FGameplayTag& InputTag);

	bool AbilityHasTag(const FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& Tag);

	UFUNCTION(NetMulticast, Unreliable)
	void MutiCastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);
};
