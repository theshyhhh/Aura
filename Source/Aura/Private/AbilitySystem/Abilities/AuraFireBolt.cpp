#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 FireBoltDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCoolDown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>火球术</>\n\n"

			// Level
			"<Small>技能等级: </><Level>%d</>\n"
			// ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"

			"<Default>发射火球，命中敌人时爆炸并造成 </>"

			// Damage
			"<Damage>%d</><Default>火焰伤害</>\n"
			"<Default>有几率造成燃烧</>"),

		                       // Values
		                       Level,
		                       ManaCost,
		                       Cooldown,
		                       FireBoltDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>火球术</>\n\n"

			// Level
			"<Small>技能等级: </><Level>%d</>\n"
			// ManaCost
			"<Small>法力消耗: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"

			"<Default>发射%d个火球，命中敌人时爆炸并造成 </>"

			// Damage
			"<Damage>%d</><Default>火焰伤害</>\n"
			"<Default>有几率造成燃烧</>"),

		                       // Values
		                       Level,
		                       ManaCost,
		                       Cooldown,
		                       FMath::Min(Level, NumOfProjectiles),
		                       FireBoltDamage);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 FireBoltDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCoolDown(Level);
	return FString::Printf(TEXT(
		// Title
		"<Title>下一等级</>\n\n"

		// Level
		"<Small>技能等级: </><Level>%d</>\n"
		// ManaCost
		"<Small>法力消耗: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"

		"<Default>发射%d个火球，命中敌人时爆炸并造成 </>"

		// Damage
		"<Damage>%d</><Default>火焰伤害</>\n"
		"<Default>有几率造成燃烧</>"),

	                       // Values
	                       Level,
	                       ManaCost,
	                       Cooldown,
	                       FMath::Min(Level, NumOfProjectiles),
	                       FireBoltDamage);
}
