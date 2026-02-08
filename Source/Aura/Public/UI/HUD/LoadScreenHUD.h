#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadScreenHUD.generated.h"

class UMVVM_LoadScreenViewModel;
class ULoadScreenUserWidget;

UCLASS()
class AURA_API ALoadScreenHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE UMVVM_LoadScreenViewModel* GetLoadScreenViewModel() const { return LoadScreenViewModel; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<ULoadScreenUserWidget> LoadScreen;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadScreenViewModel> LoadScreenViewModel;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LoadScreenClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadScreenViewModel> LoadScreenViewModeClass;
};
