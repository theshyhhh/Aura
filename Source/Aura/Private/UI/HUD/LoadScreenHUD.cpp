#include "UI/HUD/LoadScreenHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreenViewModel.h"
#include "UI/Widget/LoadScreenUserWidget.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	LoadScreenViewModel = NewObject<UMVVM_LoadScreenViewModel>(this, LoadScreenViewModeClass);
	LoadScreenViewModel->InitializeLoadSlotViewModels();

	LoadScreen = CreateWidget<ULoadScreenUserWidget>(GetWorld(), LoadScreenClass);
	LoadScreen->AddToViewport();
	LoadScreen->BlueprintInitializeWidget();

	LoadScreenViewModel->LoadData();
}
