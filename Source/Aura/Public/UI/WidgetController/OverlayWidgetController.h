#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "OverlayWidgetController.generated.h"

class UAuraUserWidget;
struct FOnAttributeChangeData;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelChangedDynamicSignature, int32, NewValue, bool, bPlayLevelUpEffect);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, const FUIWidgetRow&, Row);

UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|XP")
	FOnAttributeChangedSignature OnXPChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Level")
	FOnLevelChangedDynamicSignature OnLevelChangedDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetData")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	template <typename T>
	const T* GetDataTableRowByTag(const UDataTable* DataTable, const FGameplayTag& Tag);

	void OnXPChanged(int32 NewXP);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& InputTag,
	                       const FGameplayTag& PrevInputTag) const;
};

template <typename T>
const T* UOverlayWidgetController::GetDataTableRowByTag(const UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(),TEXT(""));
}
