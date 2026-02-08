#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlotViewModel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetSwitcherIndexSetSignature, int32, Index);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectSlotButtonEnabledSignature, bool, Enabled);

UCLASS()
class AURA_API UMVVM_LoadSlotViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/*用于改变Switcher部件显示的Widget*/
	UPROPERTY(BlueprintAssignable)
	FOnWidgetSwitcherIndexSetSignature OnWidgetSwitcherIndexSetDelegate;

	/**用于禁用被选中的插槽的按钮，激活未被选中的插槽的按钮*/
	UPROPERTY(BlueprintAssignable)
	FOnSelectSlotButtonEnabledSignature OnSelectSlotButtonEnabledDelegate;

	//更新Switcher显示的Widget
	void UpdateSlot();

	void SetLoadSlotName(const FString& InLoadSlotName);

	const FString& GetLoadSlotName() const { return LoadSlotName; }

	void SetPlayerName(const FString& InPlayerName);

	const FString& GetPlayerName() const { return PlayerName; }

	void SetMapName(const FString& InMapName);

	const FString& GetMapName() const { return MapName; }

	UPROPERTY()
	int32 SlotIndex;

	UPROPERTY()
	ESaveSlotStatus SaveSlotStatus;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FString LoadSlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FString MapName;
};
