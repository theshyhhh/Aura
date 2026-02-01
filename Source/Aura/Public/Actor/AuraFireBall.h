#pragma once

#include "CoreMinimal.h"
#include "AuraProjectile.h"
#include "AuraFireBall.generated.h"

UCLASS()
class AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()

public:
	AAuraFireBall();

	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();

	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	FDamageEffectParams ExplosionEffectParams;

protected:
	virtual void BeginPlay() override;

	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION(BlueprintCallable)
	void CauseExplosionDamage();

	UPROPERTY(EditDefaultsOnly, Category="AuraAbility|DamageEffectParams")
	float ExplosionInnerRadius;

	UPROPERTY(EditDefaultsOnly, Category="AuraAbility|DamageEffectParams")
	float ExplosionOuterRadius;

	UPROPERTY(EditDefaultsOnly, Category="AuraAbility|DamageEffectParams")
	float KnockBackMagnitude;

	UPROPERTY(EditDefaultsOnly, Category="AuraAbility|DamageEffectParams")
	float DeathImpulseMagnitude;
	
};
