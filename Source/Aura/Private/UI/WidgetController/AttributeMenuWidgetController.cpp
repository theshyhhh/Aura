#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	check(AttributeInfo)
	for (const auto& Pair : GetAuraAttributeSet()->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	OnAttributePointChangedDelegate.Broadcast(GetAuraPlayerState()->GetAttributePoint());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	for (const auto& Pair : GetAuraAttributeSet()->TagsToAttributes)
	{
		//绑定属性变更时触发的委托
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this,Pair](const FOnAttributeChangeData&)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			});
	}
	GetAuraPlayerState()->OnAttributePointChangedDelegate.AddLambda([this](int32 NewValue)-> void
	{
		OnAttributePointChangedDelegate.Broadcast(NewValue);
	});
}

void UAttributeMenuWidgetController::UpgradeAttribute(FGameplayTag AttributeTag)
{
	GetAuraAbilitySystemComponent()->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
