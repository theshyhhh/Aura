#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"


UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();

	//Primary Attributes
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);
	//Secondary Attributes
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(AuraGameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	//Secondary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);

	//注册Health变量，复制无需条件，只要值被设置了，无论是否改变，就会复制
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	if (Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter))return;
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleDamage(Props);
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleXP(Props);
	}
}

void UAuraAttributeSet::ShowDamageText(const FEffectProperties& Props, const float Damage, const bool bIsBlockedHit, const bool bIsCriticalHit) const
{
	if (Props.SourceAvatarActor != Props.TargetAvatarActor)
	{
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceController))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bIsBlockedHit, bIsCriticalHit);
		}
		else if (AAuraPlayerController* PC1 = Cast<AAuraPlayerController>(Props.TargetController))
		{
			PC1->ShowDamageNumber(Damage, Props.TargetCharacter, bIsBlockedHit, bIsCriticalHit);
		}
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetAvatarActor->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetCharacterLevel(Props.TargetCharacter);
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetAvatarActor);
		const int XPReward = UAuraAbilitySystemLibrary::GetXPRewardByClassAndLevel(Props.TargetAvatarActor, TargetClass, TargetLevel);

		FGameplayEventData Payload;
		Payload.EventTag = FAuraGameplayTags::Get().Attributes_Meta_InComingXP;
		Payload.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, FAuraGameplayTags::Get().Attributes_Meta_InComingXP, Payload);
	}
}

//On Rep Begin
//Primary Attributes
void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience)
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor)
}

//Secondary Attributes
void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor)
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration)
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance)
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance)
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage)
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance)
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration)
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration)
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

//Vital Attributes
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//通知ASC，Health被改变了，ASC会记录这些改变和旧值，方便回滚
	//当服务器认为这些更改无效的话，会回滚这些更改并撤销它们
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetMaxHealthAttribute() && bFullyRestoreHealth)
	{
		SetHealth(GetMaxHealth());
		bFullyRestoreHealth = false;
	}
	else if (Attribute == GetMaxManaAttribute() && bFullyRestoreMana)
	{
		SetMana(GetMaxMana());
		bFullyRestoreMana = false;
	}
}

//OnRepEnd

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data,
                                            FEffectProperties& Props) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this AS)

	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->
	                                                                                     AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UAuraAttributeSet::HandleXP(const FEffectProperties& Props)
{
	//IncomingXP的修改来自于经验获得者本身，它是由GA_EventBasedEffect对自身应用的GE
	const float LocalIncomingXP = GetIncomingXP();
	SetIncomingXP(0.f);
	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
	{
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);
		const int32 CurrentLevel = ICombatInterface::Execute_GetCharacterLevel(Props.SourceCharacter);
		const int32 NewLevel = IPlayerInterface::Execute_FindLevelByXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
		const int32 NumOfLevelsGained = NewLevel - CurrentLevel;
		if (NumOfLevelsGained > 0)
		{
			int32 AttributePointsReward = 0;
			int32 SpellPointsReward = 0;
			for (int32 i = CurrentLevel + 1; i <= NewLevel; i++)
			{
				AttributePointsReward += IPlayerInterface::Execute_GetAttributePointReward(Props.SourceCharacter, i);
				SpellPointsReward += IPlayerInterface::Execute_GetSpellPointReward(Props.SourceCharacter, i);
			}
			IPlayerInterface::Execute_AddPlayerLevel(Props.SourceCharacter, NumOfLevelsGained);
			IPlayerInterface::Execute_AddAttributePoint(Props.SourceCharacter, AttributePointsReward);
			IPlayerInterface::Execute_AddSpellPoint(Props.SourceCharacter, SpellPointsReward);
			IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
			//设置生命值法力为最大值
			bFullyRestoreHealth = true;
			bFullyRestoreMana = true;
		}
		IPlayerInterface::Execute_AddXP(Props.SourceCharacter, LocalIncomingXP);
	}
}

void UAuraAttributeSet::HandleDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage > 0.f)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		//检测是否死亡，如果没死播放执行受击逻辑，如果死了，执行死亡逻辑
		if (NewHealth > 0.f)
		{
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FAuraGameplayTags::Get().Effects_HitReact);
			Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(Props.EffectContextHandle
				.Get()))
			{
				const FVector KnockBackForce = AuraGameplayEffectContext->GetKnockBackForce();
				if (!KnockBackForce.IsNearlyZero())
				{
					UE_LOG(LogTemp, Warning, TEXT("KnockBackForce is zero!"));
					Props.TargetCharacter->LaunchCharacter(KnockBackForce, true, true);
				}
			}
		}
		else
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor))
			{
				CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
			}
			SendXPEvent(Props);
		}
		ShowDamageText(Props, LocalIncomingDamage, UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle),
		               UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle));
		if (UAuraAbilitySystemLibrary::IsDebuffAppliedSuccessfully(Props.EffectContextHandle))
		{
			HandleDebuff(Props);
		}
	}
}

void UAuraAttributeSet::HandleDebuff(const FEffectProperties& Props)
{
	FGameplayEffectContextHandle EffectContextHandle = Props.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(Props.SourceAvatarActor);
	const FGameplayTag DamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);
	UGameplayEffect* DebuffGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));
	DebuffGE->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DebuffGE->DurationMagnitude = FScalableFloat(DebuffDuration);
	DebuffGE->Period = DebuffFrequency;
	DebuffGE->CachedGrantedTags.AddTag(FAuraGameplayTags::Get().DamageTypeToDebuff[DamageType]);
	DebuffGE->StackingType = EGameplayEffectStackingType::AggregateBySource;
	DebuffGE->StackLimitCount = 1;
	FGameplayModifierInfo ModifierInfo = FGameplayModifierInfo();
	ModifierInfo.ModifierOp = EGameplayModOp::AddBase;
	ModifierInfo.Attribute = GetIncomingDamageAttribute();
	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	DebuffGE->Modifiers.Add(ModifierInfo);
	if (const FGameplayEffectSpec* EffectSpec = new FGameplayEffectSpec(DebuffGE, EffectContextHandle, 1.f))
	{
		FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectSpec->GetContext().Get());
		AuraGameplayEffectContext->SetDamageType(DamageType);
		Props.SourceASC->ApplyGameplayEffectSpecToTarget(*EffectSpec, Props.TargetASC);
	}
}
