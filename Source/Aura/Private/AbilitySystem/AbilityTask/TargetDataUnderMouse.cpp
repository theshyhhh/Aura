#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	//是否是本地控制的
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		//将回调函数绑定到对应的委托上
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(
			this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallBack);
		//如果数据已到达则委托直接进行广播
		if (!AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(GetAbilitySpecHandle(),GetActivationPredictionKey()))
		{
			//如果未到达，则标记等待，数据到达时，直接广播
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	//告诉系统，范围内的所有操作都该被预测，服务器允许我们在本地执行这些操作
	FScopedPredictionWindow ScopedPredictionWindow(AbilitySystemComponent.Get());

	const APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Data->HitResult);
	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(Data);
	//向服务器端发送数据，并广播一次委托，由于委托绑定回调函数和数据到达服务器的顺序不确定，所以该次广播不一定成功
	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(),
	                                                      DataHandle, FGameplayTag(),
	                                                      AbilitySystemComponent->ScopedPredictionKey);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	//广播委托后，清空数据，防止脏数据的存在
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
