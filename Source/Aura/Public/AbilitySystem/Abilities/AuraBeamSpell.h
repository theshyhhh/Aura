#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category="AuraBeamSpell")
	void SetMouseHitInfo(const FHitResult& MouseHitResult);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraBeamSpell")
	APlayerController* GetOwnerController() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraBeamSpell")
	ACharacter* GetOwnerCharacter() const;

	UPROPERTY(BlueprintReadWrite, Category="AuraBeamSpell")
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category="AuraBeamSpell")
	TWeakObjectPtr<AActor> HitActor;
};
