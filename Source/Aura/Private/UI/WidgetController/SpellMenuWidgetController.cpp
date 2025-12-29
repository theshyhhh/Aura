#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadCastAbilityInfo();
	OnSpellPointChangedMultiDelegate.Broadcast(GetAuraPlayerState()->GetSpellPoint());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraAbilitySystemComponent()->OnAbilityStatusChangedDelegate.AddLambda(
		[this](FGameplayTag AbilityTag, FGameplayTag StatusTag, int32 AbilityLevel)-> void
		{
			/*
			 * 如果当前选中的技能是技能状态更改的技能
			 * 则同步更新当前选中技能的状态，两个按钮的状态，技能描述的状态
			 */
			if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
			{
				SelectedAbility.StatusTag = StatusTag;
				bool bIsSpendSpellButtonEnabled = false;
				bool bIsEquipButtonEnabled = false;
				ShouldEnableButtons(CurrentSpellPoint, SelectedAbility.StatusTag, bIsSpendSpellButtonEnabled, bIsEquipButtonEnabled);
				FString Description;
				FString NextLevelDescription;
				GetAuraAbilitySystemComponent()->GetDescriptionByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
				OnSpellGlobeSelectedDelegate.Broadcast(bIsSpendSpellButtonEnabled, bIsEquipButtonEnabled, Description, NextLevelDescription);
			}
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityByTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		});
	GetAuraPlayerState()->OnSpellPointChangedDelegate.AddLambda([this](int32 NewValue)-> void
	{
		CurrentSpellPoint = NewValue;
		bool bIsSpendSpellButtonEnabled = false;
		bool bIsEquipButtonEnabled = false;
		ShouldEnableButtons(CurrentSpellPoint, SelectedAbility.StatusTag, bIsSpendSpellButtonEnabled, bIsEquipButtonEnabled);
		FString Description;
		FString NextLevelDescription;
		GetAuraAbilitySystemComponent()->GetDescriptionByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
		OnSpellGlobeSelectedDelegate.Broadcast(bIsSpendSpellButtonEnabled, bIsEquipButtonEnabled, Description, NextLevelDescription);
		OnSpellPointChangedMultiDelegate.Broadcast(NewValue);
	});
	GetAuraAbilitySystemComponent()->OnAbilityEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bIsWaitingForEquipSelection)
	{
		OnStopEquipDelegate.Broadcast(AbilityInfo->FindAbilityByTag(SelectedAbility.AbilityTag).AbilityType);
		bIsWaitingForEquipSelection = false;
	}
	const int32 SpellPoint = GetAuraPlayerState()->GetSpellPoint();
	const FAuraGameplayTags AuraGameplayTags = FAuraGameplayTags::Get();
	FGameplayTag AbilityStatusTag;
	const bool bIsTagValid = AbilityTag.IsValid();
	const bool bIsTagNone = AbilityTag.MatchesTagExact(AuraGameplayTags.Abilities_None);
	const FGameplayAbilitySpec* AbilitySpec = GetAuraAbilitySystemComponent()->GetAbilitySpecByTag(AbilityTag);
	const bool bIsAbilitySpecValid = AbilitySpec != nullptr;
	if (!bIsAbilitySpecValid || !bIsTagValid || bIsTagNone)
	{
		AbilityStatusTag = AuraGameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatusTag = GetAuraAbilitySystemComponent()->GetStatusTagFromAbilitySpec(*AbilitySpec);
	}
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.StatusTag = AbilityStatusTag;
	bool bIsSpendSpellButtonEnabled = false;
	bool bIsEquipButtonEnabled = false;
	ShouldEnableButtons(SpellPoint, AbilityStatusTag, bIsSpendSpellButtonEnabled, bIsEquipButtonEnabled);
	FString Description;
	FString NextLevelDescription;
	GetAuraAbilitySystemComponent()->GetDescriptionByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
	OnSpellGlobeSelectedDelegate.Broadcast(bIsSpendSpellButtonEnabled, bIsEquipButtonEnabled, Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpendSpellPointButtonPressed()
{
	GetAuraAbilitySystemComponent()->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::SpellGlobeDeselected()
{
	if (bIsWaitingForEquipSelection)
	{
		bIsWaitingForEquipSelection = false;
		OnStopEquipDelegate.Broadcast(AbilityInfo->FindAbilityByTag(SelectedAbility.AbilityTag).AbilityType);
	}
	SelectedAbility.AbilityTag = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.StatusTag = FAuraGameplayTags::Get().Abilities_Status_Locked;
	OnSpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType = AbilityInfo->FindAbilityByTag(SelectedAbility.AbilityTag).AbilityType;
	OnEquipButtonPressedDelegate.Broadcast(AbilityType);
	bIsWaitingForEquipSelection = true;
	//判断选中的技能是否已有输入的标签
	if (GetAuraAbilitySystemComponent()->GetStatusTagByAbilityTag(SelectedAbility.AbilityTag).MatchesTagExact(
		FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedInputTag = GetAuraAbilitySystemComponent()->GetInputTagByAbilityTag(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::EquippedSpellRowPressed(const FGameplayTag& InputSlotTag, const FGameplayTag& AbilityType)
{
	if (!bIsWaitingForEquipSelection)return;
	//检查技能的类型和输入插槽的类型是否相同，即是否都是主动或都是被动技能
	const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityByTag(SelectedAbility.AbilityTag).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType))return;
	GetAuraAbilitySystemComponent()->ServerEquipAbility(SelectedAbility.AbilityTag, InputSlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag,
                                                   const FGameplayTag& PrevInputTag)
{
	bIsWaitingForEquipSelection = false;
	const FAuraGameplayTags AuraTags = FAuraGameplayTags::Get();
	//清空已绑定的上一个技能快捷键，如果有的话
	FAuraAbilityInfo LastInputInfo;
	LastInputInfo.InputTag = PrevInputTag;
	LastInputInfo.StatusTag = AuraTags.Abilities_Status_Unlocked;
	LastInputInfo.AbilityTag = AuraTags.Abilities_None;
	AbilityInfoDelegate.Broadcast(LastInputInfo);
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityByTag(AbilityTag);
	Info.StatusTag = StatusTag;
	Info.InputTag = InputTag;
	AbilityInfoDelegate.Broadcast(Info);
	OnStopEquipDelegate.Broadcast(AbilityInfo->FindAbilityByTag(AbilityTag).AbilityType);
	SpellGlobeReassignDelegate.Broadcast(AbilityTag);
	SpellGlobeDeselected();
}

void USpellMenuWidgetController::ShouldEnableButtons(const int32 SpellPoint, FGameplayTag AbilityStatusTag, bool& bIsSpendSpellButtonEnabled,
                                                     bool& bIsEquipButtonEnabled)
{
	if (AbilityStatusTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped) || AbilityStatusTag.MatchesTagExact(
		FAuraGameplayTags::Get().Abilities_Status_Unlocked))
	{
		bIsEquipButtonEnabled = true;
		if (SpellPoint > 0)
		{
			bIsSpendSpellButtonEnabled = true;
		}
	}
	else if (SpellPoint > 0 && AbilityStatusTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Eligible))
	{
		bIsSpendSpellButtonEnabled = true;
	}
}
