#pragma once

#include "CoreMinimal.h"
#include "AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

protected:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& TargetLocation, const FGameplayTag& FireSocketTag, AActor* HomingTarget, const bool bOverridePitch = false,
	                      const float Pitch = 0.f);

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float FireSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float HomingAccelerationMin = 1600.f;

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float HomingAccelerationMax = 3200.f;
};
