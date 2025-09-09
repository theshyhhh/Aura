#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	//生成一个投射物，但只在服务器端执行
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); //判断是否是服务器端或者单机
	if (!bIsServer)return;
	const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)return;
	FRotator Rotation = (ProjectileTargetLocation - CombatInterface->GetCombatSocketLocation()).Rotation();
	Rotation.Pitch = 0.f;
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(CombatInterface->GetCombatSocketLocation());
	SpawnTransform.SetRotation(Rotation.Quaternion());
	check(ProjectileClass)
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
	                                                                              Cast<APawn>(GetOwningActorFromActorInfo()),
	                                                                              ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	//TODO: 给投射物一个用于造成伤害的GE Spec

	Projectile->FinishSpawning(SpawnTransform);
}
