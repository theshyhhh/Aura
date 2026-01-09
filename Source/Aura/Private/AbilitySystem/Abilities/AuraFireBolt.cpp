#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

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
		                       FMath::Min(Level, MaxNumOfProjectiles),
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
	                       FMath::Min(Level, MaxNumOfProjectiles),
	                       FireBoltDamage);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& TargetLocation, const FGameplayTag& FireSocketTag, AActor* HomingTarget,
                                     const bool bOverridePitch, const float Pitch)
{
	check(ProjectileClass)
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor->HasAuthority() || !AvatarActor->Implements<UCombatInterface>())return;
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, FireSocketTag);
	FRotator Rotation = (TargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)Rotation.Pitch = Pitch;
	UKismetSystemLibrary::DrawDebugArrow(AvatarActor, SocketLocation, SocketLocation + Rotation.Vector() * 100, 5, FLinearColor::Red, 120, 1);
	const int32 NumOfProjectiles = FMath::Min(MaxNumOfProjectiles, GetAbilityLevel());
	TArray<FRotator> FireDirections =
		UAuraAbilitySystemLibrary::GetEvenlyRotators(Rotation.Vector(), FVector::UpVector, FireSpread, NumOfProjectiles);
	for (const FRotator& Direction : FireDirections)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Direction.Quaternion());
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
		                                                                              Cast<APawn>(GetOwningActorFromActorInfo()),
		                                                                              ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Projectile->DamageEffectParams = MakeDamageEffectParamFromClassDefaults();
		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovementComponent->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			Projectile->SetHomingTargetSceneComponent(NewObject<USceneComponent>(USceneComponent::StaticClass()));
			Projectile->ProjectileMovementComponent->HomingTargetComponent->SetWorldLocation(TargetLocation);
		}
		Projectile->ProjectileMovementComponent->HomingAccelerationMagnitude = FMath::RandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovementComponent->bIsHomingProjectile = true;
		Projectile->FinishSpawning(SpawnTransform);
	}
}
