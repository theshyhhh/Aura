#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BroadcastInitialValues() const
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo)
	for (const auto& Pair : AuraAttributeSet->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	OnAttributePointChangedDelegate.Broadcast(AuraPlayerState->GetAttributePoint());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	for (const auto& Pair : AuraAttributeSet->TagsToAttributes)
	{
		//绑定属性变更时触发的委托
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this,Pair](const FOnAttributeChangeData&)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			});
	}
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnAttributePointChangedDelegate.AddLambda([this](int32 NewValue)-> void
	{
		OnAttributePointChangedDelegate.Broadcast(NewValue);
	});
}

void UAttributeMenuWidgetController::UpgradeAttribute(FGameplayTag AttributeTag)
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
