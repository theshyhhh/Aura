#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributeInfo.generated.h"

USTRUCT(BlueprintType, meta=(ToolTip="用于传递技能信息"))
struct FAuraAttributeInfo
{
	GENERATED_BODY()

	//通过Tag来识别属性
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();

	//想要在界面上显示的，就用FText
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText AttributeDescription = FText();

	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.0f;
};

UCLASS()
class AURA_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	
	// 通过Tag来从数据资产中获得用于传递属性信息的结构
	FAuraAttributeInfo FindAttributeInfoByTag(const FGameplayTag& Tag, bool bLogNotFound = false) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAuraAttributeInfo> AttributeInfos;
};
