#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAttributeSet()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAttributeSet()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAttributeSet()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAttributeSet()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPlayerState()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPlayerState()->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel)-> void
	{
		OnLevelChangedDelegate.Broadcast(NewLevel);
	});
	//绑定当对应属性值变化使，想要调用的回调函数
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)-> void
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)-> void
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)-> void
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)-> void
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);
	if (GetAuraAbilitySystemComponent())
	{
		GetAuraAbilitySystemComponent()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					//从配置文件中查找所需的Tag，如果Tag不存在就会报错
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Message")));
					//检测子Tag是否能匹配到父Tag，即传入的Tag是不是Message下的Tag
					if (!Tag.MatchesTag(MessageTag))continue;

					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		);
		GetAuraAbilitySystemComponent()->OnAbilityEquippedDelegate.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		if (GetAuraAbilitySystemComponent()->bStartupAbilityHasBeenGiven)
		{
			BroadCastAbilityInfo();
		}
		else
		{
			GetAuraAbilitySystemComponent()->OnAbilityGrantedDelegate.AddUObject(this, &UOverlayWidgetController::BroadCastAbilityInfo);
		}
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	ULevelUpInfo* LevelUpInfo = GetAuraPlayerState()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("LevelUpInfo指针为空"));

	const int32 Level = LevelUpInfo->FindLevelByXP(NewXP);
	if (Level == 1)
	{
		OnXPChangedDelegate.Broadcast(static_cast<float>(NewXP) / static_cast<float>(LevelUpInfo->LevelUpInfo[Level - 1].RequiredXP));
	}
	else
	{
		const int32 XPCurrentLevelRequired = LevelUpInfo->LevelUpInfo[Level - 1].RequiredXP;
		const int32 XPPreviousLevelRequired = LevelUpInfo->LevelUpInfo[Level - 2].RequiredXP;
		float XPPercent = static_cast<float>(NewXP - XPPreviousLevelRequired) / static_cast<float>(XPCurrentLevelRequired -
			XPPreviousLevelRequired);
		XPPercent = XPPercent > 1.0f ? 1.0f : XPPercent;
		OnXPChangedDelegate.Broadcast(XPPercent);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag,
                                                 const FGameplayTag& PrevInputTag) const
{
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
}
