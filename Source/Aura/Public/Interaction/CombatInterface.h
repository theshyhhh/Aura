#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class AURA_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual int32 GetCharacterLevel() const;

	virtual FVector GetCombatSocketLocation() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetFacingTarget(const FVector& TargetLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage() const;

	virtual void Die() =0;
};
