#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(
	const FAuraWidgetControllerParams& WidgetControllerParams)
{
	if (!OverlayWidgetController)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
	}
	return OverlayWidgetController;
}

//初始化HUD
void AAuraHUD::InitOverlayWidget(APlayerController* PlayerController, APlayerState* PlayerState,
                                 UAbilitySystemComponent* AbilitySystemComponent, UAttributeSet* AttributeSet)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget class is NULL"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller class is NULL"));
	//创建OverlayWidget
	UUserWidget* UserWidget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(UserWidget);
	checkf(OverlayWidget, TEXT("OverlayWidget创建失败"));
	//OverlayWidget设置控制器
	OverlayWidget->SetWidgetController(GetOverlayWidgetController(
		FAuraWidgetControllerParams(PlayerController, PlayerState, AbilitySystemComponent, AttributeSet)));
	OverlayWidgetController->BroadcastInitialValues();
	UserWidget->AddToViewport();
}
