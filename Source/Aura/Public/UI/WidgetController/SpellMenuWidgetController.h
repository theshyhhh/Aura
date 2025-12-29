#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSpellGlobeSelectedSignature, bool, bSpendSpellPointButtonEnabled, bool, bEquipSpellButtonEnabled,
                                              FString, Description, FString, NextLevelDescription);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipButtonPressedSignature, const FGameplayTag&, AbilityType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityTag);

struct FSelectedAbility
{
	FGameplayTag AbilityTag;

	FGameplayTag StatusTag;
};

UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;


	UPROPERTY(BlueprintAssignable, Category="GAS|SpellMenu")
	FOnPlayerStatChangedMultiSignature OnSpellPointChangedMultiDelegate;

	UFUNCTION(BlueprintCallable, Category="GAS|SpellMenu")
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	UPROPERTY(BlueprintAssignable, Category="GAS|SpellMenu")
	FOnSpellGlobeSelectedSignature OnSpellGlobeSelectedDelegate;

	UFUNCTION(BlueprintCallable, Category="GAS|SpellMenu")
	void SpendSpellPointButtonPressed();

	UFUNCTION(BlueprintCallable, Category="GAS|SpellMenu")
	void SpellGlobeDeselected();

	UFUNCTION(BlueprintCallable, Category="GAS|SpellMenu")
	void EquipButtonPressed();

	UPROPERTY(BlueprintAssignable, Category="GAS|SpellMenu")
	FOnEquipButtonPressedSignature OnEquipButtonPressedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|SpellMenu")
	FOnEquipButtonPressedSignature OnStopEquipDelegate;

	//技能装备后，取消当前选择的技能的委托
	UPROPERTY(BlueprintAssignable, Category="GAS|SpellMenu")
	FSpellGlobeReassignedSignature SpellGlobeReassignDelegate;

	UFUNCTION(blueprintCallable, Category="GAS|SpellMenu")
	void EquippedSpellRowPressed(const FGameplayTag& InputSlotTag, const FGameplayTag& AbilityType);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag,
	                       const FGameplayTag& PrevInputTag);

private:
	static void ShouldEnableButtons(int32 SpellPoint, FGameplayTag AbilityStatusTag, bool& bIsSpendSpellButtonEnabled, bool& bIsEquipButtonEnabled);

	FSelectedAbility SelectedAbility = {FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked};

	int32 CurrentSpellPoint = 0;

	bool bIsWaitingForEquipSelection = false;

	FGameplayTag SelectedInputTag = FGameplayTag();
};
