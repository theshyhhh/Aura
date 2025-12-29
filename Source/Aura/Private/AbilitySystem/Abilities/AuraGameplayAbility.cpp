#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(
		TEXT("<Default>%s</><Level>%d</>"),
		L"Default Ability Name-LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum",
		Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Next Level</><Level>%d</> \n<Default>造成更多伤害</>"), Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked until level: %d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(const int32 Level) const
{
	float ManaCost = 0.0f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(Level, ManaCost);
				break;
			}
		}
	}
	return ManaCost;
}

float UAuraGameplayAbility::GetCoolDown(const int32 Level) const
{
	float CD = 0.f;
	if (const UGameplayEffect* CDEffect = GetCooldownGameplayEffect())
	{
		CDEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(Level, CD);
	}
	return CD;
}
