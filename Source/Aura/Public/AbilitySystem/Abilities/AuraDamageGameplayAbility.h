#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem//AuraAbilityTypes.h"
#include "AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

protected:
	//要应用给目标的GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	//通过曲线表获得伤害值
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat Damage;

	UFUNCTION(BlueprintCallable, Category="Damage")
	float GetDamageByAbilityLevel() const;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffChance;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffDamage;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffFrequency;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffDuration;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DeathImpulseMagnitude;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockBackPitch = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockBackChance;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockForceBackMagnitude;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	bool bIsRadialDamage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float RadialDamageOuterRadius = 0.f;

	//对目标应用GE且使用SetByCaller传入伤害及伤害类型
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamFromClassDefaults(AActor* TargetActor = nullptr,
	                                                            FVector DamageOrigin = FVector::ZeroVector,
	                                                           bool bOverrideKnockBackOrigin = false);
};
