#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UAuraInputComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* UserObject, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc,
	                        HeldFuncType HeldFunc);

protected:
	virtual void BeginPlay() override;

public:
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* UserObject, PressedFuncType PressedFunc,
                                             ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);
	for (const FAuraInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				//给输入绑定一个回调函数，当输入按下时，会调用这个回调函数，且传入和该输入对应的标签，我们就能知道是哪个输入触发了该回调函数
				BindAction(Action.InputAction, ETriggerEvent::Started, UserObject, PressedFunc, Action.InputTag);
			}
			if (ReleasedFunc)
			{
				//松开
				BindAction(Action.InputAction, ETriggerEvent::Completed, UserObject, ReleasedFunc, Action.InputTag);
			}
			if (HeldFunc)
			{
				//持续按下
				BindAction(Action.InputAction, ETriggerEvent::Triggered, UserObject, HeldFunc, Action.InputTag);
			}
		}
	}
}
