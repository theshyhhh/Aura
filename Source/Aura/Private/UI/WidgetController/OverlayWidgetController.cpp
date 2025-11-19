#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues() const
{
	Super::BroadcastInitialValues();
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);

	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	//绑定当对应属性值变化使，想要调用的回调函数
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)-> void
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)-> void
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)-> void
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)-> void
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		AuraASC->EffectAssetTags.AddLambda(
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
		if (AuraASC->bStartupAbilityHasBeenGiven)
		{
			OnInitializeStartupAbility();
		}
		else
		{
			AuraASC->OnAbilityGrantedDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbility);
		}
	}
}

void UOverlayWidgetController::OnInitializeStartupAbility()
{
	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	FForEachAbility Delegate;
	Delegate.BindLambda([this,AuraASC](const FGameplayAbilitySpec& AbilitySpec)-> void
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityByTag(AuraASC->GetTagFromAbilitySpec(AbilitySpec));
			Info.InputTag = AuraASC->GetInputTagFromAbilitySpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		}
	);
	//遍历所有能力并通过委托传递AbilitySpec，在通过委托广播能力的信息给UI控件
	AuraASC->ForEachAbility(Delegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	ULevelUpInfo* LevelUpInfo = Cast<AAuraPlayerState>(PlayerState)->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("LevelUpInfo指针为空"));

	const int32 Level = LevelUpInfo->FindLevelByXP(NewXP);
	if (Level == 1)
	{
		OnXPChangedDelegate.Broadcast(NewXP / LevelUpInfo->LevelUpInfo[Level - 1].RequiredXP);
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
