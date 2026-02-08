#include "Game/AuraGameModeBase.h"

#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlotViewModel.h"

void AAuraGameModeBase::SaveSlotData(const UMVVM_LoadSlotViewModel* LoadSlotViewModel, int32 SlotIndex) const
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlotViewModel->GetLoadSlotName(), SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlotViewModel->GetLoadSlotName(), SlotIndex);
	}
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSavaGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlotViewModel->GetPlayerName();
	LoadScreenSaveGame->SaveSlotStatus = LoadSlotViewModel->SaveSlotStatus;
	LoadScreenSaveGame->MapName = LoadSlotViewModel->GetMapName();
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlotViewModel->GetLoadSlotName(), SlotIndex);
}

void AAuraGameModeBase::DeleteSlotData(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSavaGameClass);
	}
	return Cast<ULoadScreenSaveGame>(SaveGameObject);
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlotViewModel* LoadSlotVM)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this,Maps[LoadSlotVM->GetMapName()]);
}
