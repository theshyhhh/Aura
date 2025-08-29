#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoByTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo& AttributeInfo : AttributeInfos)
	{
		if (AttributeInfo.AttributeTag.MatchesTagExact(Tag))
		{
			return AttributeInfo;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s]中没有找到[%s]的信息"), *GetNameSafe(this), *Tag.ToString());
	}
	return FAuraAttributeInfo();
}
