#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
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
	const FVector CombatSocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(), FAuraGameplayTags::Get().Montage_Attack_Weapon);
	const FRotator Rotation = (ProjectileTargetLocation - CombatSocketLocation).Rotation();
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(CombatSocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());
	check(ProjectileClass)
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
	                                                                              Cast<APawn>(GetOwningActorFromActorInfo()),
	                                                                              ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	const UAbilitySystemComponent* SourceAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	FGameplayEffectContextHandle ContextHandle = SourceAsc->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(Projectile);
	const FGameplayEffectSpecHandle SpecHandle = SourceAsc->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);
	for (const auto& Pair : DamageTypes)
	{
		const float ScaleDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaleDamage);
	}
	Projectile->SetOwner(ContextHandle.GetEffectCauser());
	Projectile->DamageEffectSpecHandle = SpecHandle;
	Projectile->FinishSpawning(SpawnTransform);
}
