#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

float UAuraDamageGameplayAbility::GetDamageByAbilityLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

void UAuraDamageGameplayAbility::CauseDamage(AActor* Target)
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1);
	const float DamageScale = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, DamageScale);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),
	                                                                          UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamFromClassDefaults(AActor* TargetActor,
                                                                                       FVector DamageOrigin,
                                                                                       bool bOverrideKnockBackOrigin)
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceGameplayAbilitySystem = GetAbilitySystemComponentFromActorInfo();
	Params.TargetGameplayAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockBackMagnitude = KnockForceBackMagnitude;
	Params.KnockBackChance = KnockBackChance;
	if (IsValid(TargetActor))
	{
		FRotator DamageRotation = (TargetActor->GetActorLocation() - (bOverrideKnockBackOrigin
			                           ? DamageOrigin
			                           : GetAvatarActorFromActorInfo()->GetActorLocation())).Rotation();
		DamageRotation.Pitch = KnockBackPitch;
		Params.KnockBackForce = KnockForceBackMagnitude * DamageRotation.Vector();
		Params.DeathImpulse = DeathImpulseMagnitude * DamageRotation.Vector();
	}
	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
		Params.RadialDamageOrigin = DamageOrigin;
	}
	return Params;
}
