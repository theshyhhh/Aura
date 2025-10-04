#pragma once

#include "CoreMinimal.h"
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
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	//对目标应用GE且使用SetByCaller传入伤害及伤害类型
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* Target);
};
