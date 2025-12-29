#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

void UAuraWidgetController::SetWidgetControllerParams(const FAuraWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}

void UAuraWidgetController::BroadCastAbilityInfo()
{
	FForEachAbility Delegate;
	Delegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)-> void
		{
			//通过技能的标签查找技能信息，并赋予输入标签，广播给UI
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityByTag(UAuraAbilitySystemComponent::GetTagFromAbilitySpec(AbilitySpec));
			Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromAbilitySpec(AbilitySpec);
			Info.StatusTag = UAuraAbilitySystemComponent::GetStatusTagFromAbilitySpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		}
	);
	//遍历所有能力并通过委托传递AbilitySpec，在通过委托广播能力的信息给UI控件
	GetAuraAbilitySystemComponent()->ForEachAbility(Delegate);
}

AAuraPlayerController* UAuraWidgetController::GetAuraPlayerController()
{
	if (AuraPlayerController == nullptr)
	{
		AuraPlayerController = Cast<AAuraPlayerController>(PlayerController);
	}
	return AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPlayerState()
{
	if (AuraPlayerState == nullptr)
	{
		AuraPlayerState = Cast<AAuraPlayerState>(PlayerState);
	}
	return AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraAbilitySystemComponent()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAttributeSet()
{
	if (AuraAttributeSet == nullptr)
	{
		AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	}
	return AuraAttributeSet;
}
