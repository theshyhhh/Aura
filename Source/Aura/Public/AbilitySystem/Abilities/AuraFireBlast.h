#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireBall;

UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AuraFireBlast")
	int32 FireBallNum = 10;

	UFUNCTION(BlueprintCallable, Category="AuraFireBlast")
	TArray<AAuraFireBall*> SpawnFireBalls();

private:
	UPROPERTY(EditDefaultsOnly, Category="AuraFireBlast")
	TSubclassOf<AAuraFireBall> AuraFireBallClass;
};
