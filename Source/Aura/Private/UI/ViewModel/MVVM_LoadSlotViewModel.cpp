#include "UI/ViewModel/MVVM_LoadSlotViewModel.h"

void UMVVM_LoadSlotViewModel::UpdateSlot()
{
	const int32 WidgetSwitcherIndex = static_cast<int32>(SaveSlotStatus);
	OnWidgetSwitcherIndexSetDelegate.Broadcast(WidgetSwitcherIndex);
}

void UMVVM_LoadSlotViewModel::SetLoadSlotName(const FString& InLoadSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, InLoadSlotName);
}

void UMVVM_LoadSlotViewModel::SetPlayerName(const FString& InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadSlotViewModel::SetMapName(const FString& InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}
