#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
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
	float KnockBackChance;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockForceBackMagnitude;

	//对目标应用GE且使用SetByCaller传入伤害及伤害类型
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamFromClassDefaults(AActor* TargetActor = nullptr) const;
};
