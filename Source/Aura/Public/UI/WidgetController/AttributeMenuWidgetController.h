#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayTag;
struct FGameplayAttribute;
class UAttributeInfo;
struct FAuraAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&, AuraAttributeInfo);

UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	//广播初始值
	virtual void BroadcastInitialValues() const override;

	//绑定委托
	virtual void BindCallbacksToDependencies() override;

	//向属性菜单UI广播属性信息的委托
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|AttributePoint")
	FOnPlayerStatChangedMultiSignature OnAttributePointChangedDelegate;

	UFUNCTION(BlueprintCallable, Category="GAS|AttributePoint")
	void UpgradeAttribute(FGameplayTag AttributeTag);

protected:
	//存储属性菜单所需要的属性信息的数据资产
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

private:
	//通过标签获取属性信息并广播
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};
