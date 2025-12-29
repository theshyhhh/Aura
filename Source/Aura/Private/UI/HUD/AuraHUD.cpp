#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FAuraWidgetControllerParams& WidgetControllerParams)
{
	if (!OverlayWidgetController)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		OverlayWidgetController->BindCallbacksToDependencies(); //控制器初始化后就可以将控制器的回调函数绑定到ASC的属性改变的委托上了
	}
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FAuraWidgetControllerParams& WidgetControllerParams)
{
	if (!AttributeMenuWidgetController)
	{
		check(AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		AttributeMenuWidgetController->BindCallbacksToDependencies();
	}
	return AttributeMenuWidgetController;
}

USpellMenuWidgetController* AAuraHUD::GetSpellMenuWidgetController(const FAuraWidgetControllerParams& WidgetControllerParams)
{
	if (!SpellMenuWidgetController)
	{
		check(SpellMenuWidgetControllerClass);
		SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
		SpellMenuWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		SpellMenuWidgetController->BindCallbacksToDependencies();
	}
	return SpellMenuWidgetController;
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
	OverlayWidget->SetWidgetController(
		GetOverlayWidgetController(FAuraWidgetControllerParams(PlayerController, PlayerState, AbilitySystemComponent, AttributeSet)));
	OverlayWidgetController->BroadcastInitialValues(); //只能在OverlayWidget设置了控制器，绑定了回调函数后广播
	UserWidget->AddToViewport();
}
