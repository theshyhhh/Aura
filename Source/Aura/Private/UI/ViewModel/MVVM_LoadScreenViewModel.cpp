#include "UI/ViewModel/MVVM_LoadScreenViewModel.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlotViewModel.h"

void UMVVM_LoadScreenViewModel::InitializeLoadSlotViewModels()
{
	LoadSlotViewModel_0 = NewObject<UMVVM_LoadSlotViewModel>(this, LoadSlotViewModelClass);
	LoadSlotViewModel_0->SetLoadSlotName("LoadSlot_0");
	LoadSlotViewModel_0->SlotIndex = 0;
	IndexToLoadSlotViewModel.Add(0, LoadSlotViewModel_0);
	LoadSlotViewModel_1 = NewObject<UMVVM_LoadSlotViewModel>(this, LoadSlotViewModelClass);
	LoadSlotViewModel_1->SetLoadSlotName("LoadSlot_1");
	LoadSlotViewModel_1->SlotIndex = 1;
	IndexToLoadSlotViewModel.Add(1, LoadSlotViewModel_1);
	LoadSlotViewModel_2 = NewObject<UMVVM_LoadSlotViewModel>(this, LoadSlotViewModelClass);
	LoadSlotViewModel_2->SetLoadSlotName("LoadSlot_2");
	LoadSlotViewModel_2->SlotIndex = 2;
	IndexToLoadSlotViewModel.Add(2, LoadSlotViewModel_2);
}

void UMVVM_LoadScreenViewModel::SetLoadSlotNum(int32 InLoadSlotNum)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotNum, InLoadSlotNum);
}

UMVVM_LoadSlotViewModel* UMVVM_LoadScreenViewModel::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return IndexToLoadSlotViewModel.FindChecked(Index);
}

void UMVVM_LoadScreenViewModel::NewSlotButtonClicked(int32 SlotIndex, const FString& EnteredName)
{
	IndexToLoadSlotViewModel[SlotIndex]->SetPlayerName(EnteredName);
	IndexToLoadSlotViewModel[SlotIndex]->SaveSlotStatus = ESaveSlotStatus::Taken;
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	IndexToLoadSlotViewModel[SlotIndex]->SetMapName(AuraGameMode->GetDefaultMapName());
	AuraGameMode->SaveSlotData(IndexToLoadSlotViewModel[SlotIndex], SlotIndex);
	IndexToLoadSlotViewModel[SlotIndex]->UpdateSlot();
}

void UMVVM_LoadScreenViewModel::NewGameButtonClicked(int32 SlotIndex)
{
	IndexToLoadSlotViewModel[SlotIndex]->OnWidgetSwitcherIndexSetDelegate.Broadcast(1);
}

void UMVVM_LoadScreenViewModel::SelectSlotButtonClicked(int32 SlotIndex)
{
	OnSlotSelectedDelegate.Broadcast();
	for (const TPair<int, UMVVM_LoadSlotViewModel*>& Pair : IndexToLoadSlotViewModel)
	{
		Pair.Value->OnSelectSlotButtonEnabledDelegate.Broadcast(SlotIndex != Pair.Key);
	}
	SelectedSlotIndex = SlotIndex;
}

void UMVVM_LoadScreenViewModel::PlayButtonClicked()
{
	if (SelectedSlotIndex < 0 || SelectedSlotIndex > 2)return;
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	AuraGameModeBase->TravelToMap(IndexToLoadSlotViewModel[SelectedSlotIndex]);
}

void UMVVM_LoadScreenViewModel::LoadData()
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (const TPair<int, UMVVM_LoadSlotViewModel*>& Pair : IndexToLoadSlotViewModel)
	{
		ULoadScreenSaveGame* LoadScreenSaveGame = AuraGameModeBase->GetSaveSlotData(Pair.Value->GetLoadSlotName(), Pair.Value->SlotIndex);
		Pair.Value->SaveSlotStatus = LoadScreenSaveGame->SaveSlotStatus;
		Pair.Value->SetPlayerName(LoadScreenSaveGame->PlayerName);
		Pair.Value->SetMapName(LoadScreenSaveGame->MapName);
		Pair.Value->UpdateSlot();
	}
}

void UMVVM_LoadScreenViewModel::DeleteButtonClicked()
{
	if (SelectedSlotIndex < 0 || SelectedSlotIndex > 2)return;
	UMVVM_LoadSlotViewModel* LoadSlotViewModel = IndexToLoadSlotViewModel[SelectedSlotIndex];
	AAuraGameModeBase::DeleteSlotData(LoadSlotViewModel->GetLoadSlotName(), SelectedSlotIndex);
	LoadSlotViewModel->SaveSlotStatus = ESaveSlotStatus::Vacant;
	LoadSlotViewModel->UpdateSlot();
	LoadSlotViewModel->OnSelectSlotButtonEnabledDelegate.Broadcast(true);
	SelectedSlotIndex = -1;
}
