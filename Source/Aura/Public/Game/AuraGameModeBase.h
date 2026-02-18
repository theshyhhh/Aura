#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class ULoadScreenSaveGame;
class USaveGame;
class UMVVM_LoadSlotViewModel;
class UAbilityInfo;
class UCharacterClassInfo;

UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void SaveSlotData(const UMVVM_LoadSlotViewModel* LoadSlotViewModel, int32 SlotIndex) const;

	void SaveWorldState(UWorld* World) const;

	void LoadWorldState(UWorld* World) const;

	static void DeleteSlotData(const FString& SlotName, int32 SlotIndex);

	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;

	FORCEINLINE FString GetDefaultMapName() const { return DefaultMapName; }

	FORCEINLINE FName GetDefaultPlayerStartTag() const { return DefaultPlayerStartTag; }

	void TravelToMap(UMVVM_LoadSlotViewModel* LoadSlotVM);

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	//地图名与地图的映射
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	//默认地图名称
	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	//默认出生地点Tag
	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSavaGameClass;

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAbilityInfo> AbilityInfo;
};
