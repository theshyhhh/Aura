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

	UFUNCTION(BlueprintCallable, Category="AuraBeamSpell")
	void TraceFirstTarget(const FVector& StartLoc, const FVector& EndLoc);

	UFUNCTION(BlueprintCallable, Category="AuraBeamSpell")
	void StoreAdditionalTarget(TArray<AActor*>& OutAdditionalActors);

	UPROPERTY(EditDefaultsOnly, Category="AuraBeamSpell")
	int32 MaxShockTargetNum = 5;

	UFUNCTION(BlueprintImplementableEvent, Category="AuraBeamSpell")
	void OnPrimaryTargetDeath(AActor* DeadActor);

	UFUNCTION(BlueprintImplementableEvent, Category="AuraBeamSpell")
	void OnAdditionalTargetDeath(AActor* DeadActor);
};
