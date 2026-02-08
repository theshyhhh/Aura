#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "MVVM_LoadScreenViewModel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlotSelectedSignature);

class UMVVM_LoadSlotViewModel;

UCLASS()
class AURA_API UMVVM_LoadScreenViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeLoadSlotViewModels();

	void SetLoadSlotNum(int32 InLoadSlotNum);

	int32 GetLoadSlotNum() const { return LoadSlotNum; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMVVM_LoadSlotViewModel* GetLoadSlotViewModelByIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonClicked(int32 SlotIndex, const FString& EnteredName);

	UFUNCTION(BlueprintCallable)
	void NewGameButtonClicked(int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonClicked(int32 SlotIndex);
	
	UFUNCTION(BlueprintCallable)
	void PlayButtonClicked();

	UFUNCTION(BlueprintCallable)
	void DeleteButtonClicked();
	
	void LoadData();

	/*有选择存档按钮按下时广播激活开始和删除按钮*/
	UPROPERTY(BlueprintAssignable)
	FOnSlotSelectedSignature OnSlotSelectedDelegate;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	int32 LoadSlotNum;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlotViewModel> LoadSlotViewModelClass;

	UPROPERTY()
	TMap<int32, UMVVM_LoadSlotViewModel*> IndexToLoadSlotViewModel;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlotViewModel> LoadSlotViewModel_0;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlotViewModel> LoadSlotViewModel_1;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlotViewModel> LoadSlotViewModel_2;

	int32 SelectedSlotIndex = -1;
};
