#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UAuraAbilitySystemComponent* OwnerASC = Cast<UAuraAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		OwnerASC->OnPassiveEffectActivatedDelegate.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
		if (OwnerASC->bStartupAbilityHasBeenGiven)
		{
			if (OwnerASC->GetStatusTagByAbilityTag(PassiveAbilityTag).MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
			{
				Activate();
			}
		}
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
			{
				AuraASC->OnPassiveEffectActivatedDelegate.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
				if (AuraASC->bStartupAbilityHasBeenGiven)
				{
					if (AuraASC->GetStatusTagByAbilityTag(PassiveAbilityTag).MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
					{
						Activate();
					}
				}
			}
		});
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (!AbilityTag.MatchesTagExact(PassiveAbilityTag))return;
	if (bActivate && !IsActive())
	{
		Activate();
	}
	else if (!bActivate && IsActive())
	{
		Deactivate();
	}
}
