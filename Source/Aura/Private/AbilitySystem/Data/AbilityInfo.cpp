#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannel.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityByTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FAuraAbilityInfo& AbilityInfo : AbilityInformation)
	{
		if (AbilityInfo.AbilityTag == AbilityTag)
		{
			return AbilityInfo;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("FindAbilityByTag：%s技能 未从AbilityInfo:%s找到"), *AbilityTag.ToString(), *GetNameSafe(this));
	}
	return FAuraAbilityInfo();
}
