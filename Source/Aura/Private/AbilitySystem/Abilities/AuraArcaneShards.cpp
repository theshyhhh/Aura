// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraArcaneShards.h"

FString UAuraArcaneShards::GetDescription(int32 Level)
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

		"<Default>选择一块区域爆发</><Small>%d</><Default>个法术碎片，每块碎片可造成 </>"

		// Damage
		"<Damage>%d</><Default>法术伤害，伤害随着到碎片中心的距离的增加变低</>\n"),

						   // Values
						   Level,
						   ManaCost,
						   Cooldown,
						   FMath::Min(Level, MaxShardsNum),
						   DamageAtLevel);
}

FString UAuraArcaneShards::GetNextLevelDescription(int32 Level)
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

		"<Default>选择一块区域爆发</><Small>%d</><Default>个法术碎片，每块碎片可造成 </>"

		// Damage
		"<Damage>%d</><Default>法术伤害，伤害随着到碎片中心的距离的增加变低</>\n"),

						   // Values
						   Level,
						   ManaCost,
						   Cooldown,
						   FMath::Min(Level, MaxShardsNum),
						   DamageAtLevel);
}
