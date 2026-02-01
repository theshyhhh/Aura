#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const int32 FireBlastDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCoolDown(Level);
	return FString::Printf(TEXT(
		// Title
		"<Title>火焰爆炸</>\n\n"

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
	                       FireBallNum,
	                       FireBlastDamage);
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
{
	const int32 FireBlastDamage = Damage.GetValueAtLevel(Level);
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
	                       FireBallNum,
	                       FireBlastDamage);
}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<AAuraFireBall*> SpawnFireBalls;

	TArray<FRotator> SpawnRotators = UAuraAbilitySystemLibrary::GetEvenlyRotators(Forward, FVector::UpVector, 360.f, FireBallNum);
	for (const FRotator& Rotator : SpawnRotators)
	{
		FTransform SpawnTransform = FTransform::Identity;
		SpawnTransform.SetRotation(Rotator.Quaternion());
		SpawnTransform.SetLocation(Location);
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(AuraFireBallClass,
		                                                                        SpawnTransform,
		                                                                        CurrentActorInfo->PlayerController.Get(),
		                                                                        CurrentActorInfo->PlayerController->GetPawn(),
		                                                                        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		FireBall->DamageEffectParams = MakeDamageEffectParamFromClassDefaults();
		FireBall->ExplosionEffectParams = MakeDamageEffectParamFromClassDefaults();
		SpawnFireBalls.Add(FireBall);
		FireBall->FinishSpawning(SpawnTransform);
	}
	return SpawnFireBalls;
}
