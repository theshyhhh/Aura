#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannel.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectAppliedToSelf);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			//将初始技能拥有的初始输入标签传入到GA_Spec中
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilityHasBeenGiven = true;
	OnAbilityGrantedDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData)
{
	for (const FAbilitySaveInfo& AbilitySaveData : SaveData->SavedAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilitySaveData.AbilityClass, AbilitySaveData.AbilityLevel);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySaveData.AbilityInputTag);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySaveData.AbilityStatus);
		if (AbilitySaveData.AbilityType == FAuraGameplayTags::Get().Abilities_Type_Offensive)
		{
			GiveAbility(AbilitySpec);
		}
		else if (AbilitySaveData.AbilityType == FAuraGameplayTags::Get().Abilities_Type_Passive)
		{
			AbilitySaveData.AbilityStatus == FAuraGameplayTags::Get().Abilities_Status_Equipped
				? GiveAbilityAndActivateOnce(AbilitySpec)
				: GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilityHasBeenGiven = true;
	OnAbilityGrantedDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())return;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	//遍历可激活的能力
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			//在GA_Spec上设置一个bool标记，用来跟踪按键是否按下
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				UGameplayAbility* Instance = AbilitySpec.GetAbilityInstances().Last();
				FPredictionKey OriginalPredictionKey = Instance->GetCurrentActivationInfo().GetActivationPredictionKey();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle,
				                      OriginalPredictionKey);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())return;
	//遍历可激活的能力
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			//在GA_Spec上设置一个bool标记，用来跟踪按键是否按下
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				//如果能力未被激活，激活能力
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())return;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			//在GA_Spec上设置一个bool标记，用来跟踪按键是否按下
			AbilitySpecInputReleased(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				UGameplayAbility* Instance = AbilitySpec.GetAbilityInstances().Last();
				FPredictionKey OriginalPredictionKey = Instance->GetCurrentActivationInfo().GetActivationPredictionKey();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle,
				                      OriginalPredictionKey);
			}
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	//锁住Ability列表，防止在遍历期间修改列表
	FScopedAbilityListLock AbilityListLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("%hs中委托执行失败"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const
{
	FGameplayTag AbilityTag = GetAbilityTagFromAbilitySpec(AbilitySpec);
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityByTag(AbilityTag);
	return Info.AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Type_Passive);
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetAbilitySpecByTag(FGameplayTag AbilityTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability->GetAssetTags())
		{
			if (Tag.MatchesTagExact(AbilityTag))return &AbilitySpec;
		}
	}
	return nullptr;
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagByAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(AbilityTag))
	{
		return GetStatusTagFromAbilitySpec(*AbilitySpec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagByAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(AbilityTag))
	{
		return GetInputTagFromAbilitySpec(*AbilitySpec);
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetAbilitySpecByInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			return &Spec;
		}
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsInputSlotEmpty(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			return false;
		}
	}
	return true;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(FGameplayTag AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetPlayerAttributePoint(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatus(int32 Level)
{
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (Level < Info.LevelRequirement)continue;
		if (GetAbilitySpecByTag(Info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.AbilityClass, 1);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			//MarkAbilitySpecDirty(AbilitySpec);
			ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible, 1);
		}
	}
}

bool UAuraAbilitySystemComponent::GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
                                                             FString& OutNextLevelDescription)
{
	if (const FGameplayAbilitySpec* Spec = GetAbilitySpecByTag(AbilityTag))
	{
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(Spec->Ability))
		{
			OutDescription = AuraAbility->GetDescription(Spec->Level);
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(Spec->Level + 1);
			return true;
		}
	}
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))
	{
		OutDescription = FString();
	}
	else
	{
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityByTag(AbilityTag).LevelRequirement);
	}
	OutNextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
                                                                    const FGameplayTag& InputTag,
                                                                    const FGameplayTag& PrevInputTag)
{
	OnAbilityEquippedDelegate.Broadcast(AbilityTag, StatusTag, InputTag, PrevInputTag);
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	if (FGameplayAbilitySpec* Spec = GetAbilitySpecByTag(AbilityTag))
	{
		const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
		const FGameplayTag PreviousInputTag = GetInputTagFromAbilitySpec(*Spec);
		const FGameplayTag StatusTag = GetStatusTagFromAbilitySpec(*Spec);
		if (StatusTag.MatchesTagExact(AuraGameplayTags.Abilities_Status_Unlocked) || StatusTag.MatchesTagExact(
			AuraGameplayTags.Abilities_Status_Equipped))
		{
			if (FGameplayAbilitySpec* PrevAbilitySpec = GetAbilitySpecByInputTag(InputTag))
			{
				if (!AbilityTag.MatchesTagExact(GetAbilityTagFromAbilitySpec(*PrevAbilitySpec)))
				{
					if (IsPassiveAbility(*PrevAbilitySpec))
					{
						//结束该输入标签装备的原有的被动技能
						OnPassiveAbilityDeactivatedDelegate.Broadcast(GetAbilityTagFromAbilitySpec(*PrevAbilitySpec));
						MutiCastActivatePassiveEffect(GetAbilityTagFromAbilitySpec(*PrevAbilitySpec), false);
					}
					ClearAbilitySpecInputTags(PrevAbilitySpec);
				}
			}
			if (!Spec->GetDynamicSpecSourceTags().HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
			{
				if (IsPassiveAbility(*Spec))
				{
					Spec->GetDynamicSpecSourceTags().RemoveTag(GetStatusTagFromAbilitySpec(*Spec));
					Spec->GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
					//如果该被动技能未被装备，则激活这个技能
					TryActivateAbility(Spec->Handle);
					MutiCastActivatePassiveEffect(GetAbilityTagFromAbilitySpec(*Spec), true);
				}
			}
			ClearAbilitySpecInputTags(Spec);
			Spec->GetDynamicSpecSourceTags().AddTag(InputTag);
			// //搜索持有该输入标签的AbilitySpec，并将这个标签从AbilitySpec上移除
			// ClearInputTagByInputTag(InputTag);
			// //清除这个Spec原有的输入标签
			// ClearAbilitySpecInputTags(Spec);
			// //将新的输入标签绑定到Spec上
			// Spec->GetDynamicSpecSourceTags().AddTag(InputTag);
			// //如果技能为已解锁未装备状态，则更改为装备状态
			// if (StatusTag.MatchesTagExact(AuraGameplayTags.Abilities_Status_Unlocked))
			// {
			// 	Spec->GetDynamicSpecSourceTags().RemoveTag(StatusTag);
			// 	Spec->GetDynamicSpecSourceTags().AddTag(AuraGameplayTags.Abilities_Status_Equipped);
			// }
			MarkAbilitySpecDirty(*Spec);
			ClientEquipAbility(AbilityTag, AuraGameplayTags.Abilities_Status_Equipped, InputTag, PreviousInputTag);
		}
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(AbilityTag);
	if (AbilitySpec == nullptr || !GetAvatarActor()->Implements<UPlayerInterface>())return;
	IPlayerInterface::Execute_AddSpellPoint(GetAvatarActor(), -1);
	FGameplayTag AbilityStatus = GetStatusTagFromAbilitySpec(*AbilitySpec);
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	if (AbilityStatus.MatchesTagExact(AuraTags.Abilities_Status_Eligible))
	{
		AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(AuraTags.Abilities_Status_Eligible);
		AbilitySpec->GetDynamicSpecSourceTags().AddTag(AuraTags.Abilities_Status_Unlocked);
		AbilityStatus = GetStatusTagFromAbilitySpec(*AbilitySpec);
	}
	else if (AbilityStatus.MatchesTagExact(AuraTags.Abilities_Status_Unlocked) || AbilityStatus.MatchesTagExact(AuraTags.Abilities_Status_Equipped))
	{
		AbilitySpec->Level += 1;
	}
	ClientUpdateAbilityStatus(AbilityTag, AbilityStatus, AbilitySpec->Level);
	MarkAbilitySpecDirty(*AbilitySpec);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bStartupAbilityHasBeenGiven)
	{
		bStartupAbilityHasBeenGiven = true;
		OnAbilityGrantedDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClearAbilitySpecInputTags(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag InputTag = GetInputTagFromAbilitySpec(*Spec);
	Spec->GetDynamicSpecSourceTags().RemoveTag(InputTag);
}

void UAuraAbilitySystemComponent::ClearInputTagByInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasTag(&Spec, InputTag))
		{
			ClearAbilitySpecInputTags(&Spec);
		}
	}
}

bool UAuraAbilitySystemComponent::AbilityHasTag(const FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& Tag)
{
	return AbilitySpec->GetDynamicSpecSourceTags().HasTagExact(Tag);
}

void UAuraAbilitySystemComponent::MutiCastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag, bool bActivate)
{
	OnPassiveEffectActivatedDelegate.Broadcast(AbilityTag, bActivate);
}

void UAuraAbilitySystemComponent::ClientEffectAppliedToSelf_Implementation(UAbilitySystemComponent* ASC,
                                                                           const FGameplayEffectSpec& EffectSpec,
                                                                           FActiveGameplayEffectHandle ActiveEffectHandle) const
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(FGameplayTag AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.0f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	IPlayerInterface::Execute_AddAttributePoint(GetAvatarActor(), -1);
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(FGameplayTag AbilityTag, FGameplayTag StatusTag, int32 AbilityLevel)
{
	OnAbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}
