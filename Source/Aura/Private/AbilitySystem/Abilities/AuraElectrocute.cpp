// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraElectrocute.h"

FString UAuraElectrocute::GetDescription(int32 Level)
{
	const int32 DamageAtLevel = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCoolDown(Level);
	return FString::Printf(TEXT(
		// Title
		"<Title>当前等级</>\n\n"

		// Level
		"<Small>技能等级: </><Level>%d</>\n"
		// ManaCost
		"<Small>法力消耗: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"

		"<Default>发射闪电链，命中敌人时额外链接附近</><Small>%d</><Default>个目标，每</><Time>%.1f</><Default>秒可造成 </>"

		// Damage
		"<Damage>%d</><Default>闪电伤害</>\n"
		"<Default>技能结束有几率造成眩晕</>"),

	                       // Values
	                       Level,
	                       ManaCost,
	                       Cooldown,
	                       FMath::Min(Level, MaxShockTargetNum),
	                       DamageDeltaTime,
	                       DamageAtLevel);
}

FString UAuraElectrocute::GetNextLevelDescription(int32 Level)
{
	const int32 DamageAtLevel = Damage.GetValueAtLevel(Level);
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

		"<Default>发射闪电链，命中敌人时额外链接附近</><Small>%d</><Default>个目标，每</><Time>%.1f</><Default>秒可造成 </>"

		// Damage
		"<Damage>%d</><Default>闪电伤害</>\n"
		"<Default>技能结束有几率造成眩晕</>"),

	                       // Values
	                       Level,
	                       ManaCost,
	                       Cooldown,
	                       FMath::Min(Level, MaxShockTargetNum),
	                       DamageDeltaTime,
	                       DamageAtLevel);
}
