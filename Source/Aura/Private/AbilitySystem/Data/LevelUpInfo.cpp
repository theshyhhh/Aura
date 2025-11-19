#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelByXP(const int32 XP) const
{
	int32 Level = 1;
	while (true)
	{
		if (LevelUpInfo.Num() < Level)return LevelUpInfo.Num() + 1;
		if (XP >= LevelUpInfo[Level - 1].RequiredXP)
		{
			Level++;
		}
		else return Level;
	}
}
