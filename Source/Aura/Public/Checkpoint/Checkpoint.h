#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"
#include "Checkpoint.generated.h"

class USphereComponent;

UCLASS()
class AURA_API ACheckpoint : public APlayerStart, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	ACheckpoint(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(SaveGame, BlueprintReadWrite)
	bool bReached = false;

	//SaveInterface Begin

	virtual void LoadActor_Implementation() override;

	virtual bool ShouldSetTransform_Implementation() override;

	//SaveInterface End

	//HighlightInterface Begin

	virtual void GetMoveToLocation_Implementation(FVector& OutLocation) override;

	virtual void HighlightActor_Implementation() override;

	virtual void UnhighlightActor_Implementation() override;

	//HighlightInterface End

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                                  int32 OtherBodyIndex,
	                                  bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterial);

	UFUNCTION(BlueprintCallable)
	void HandleGlowEffect();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> InteractionLocation;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(EditDefaultsOnly)
	bool bSaveProgress = true;
};
