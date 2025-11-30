#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

//角色职业
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,
	Warrior,
	Ranger
};

//职业默认信息
USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

public:
	//每个职业独有的，应用后初始化默认主要属性的GE
	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	//每个职业的能力
	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	//每个职业怪物死亡奖励的经验
	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	FScalableFloat XPReward = FScalableFloat();
};

UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	//所有职业共用的，应用后初始化默认次要属性的GE
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	//所有职业共用的，应用后初始化默认主要属性的GE
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	//各个职业的主要属性GE和技能
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

	//公共的游戏能力，如受击反应等
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	//伤害计算系数
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficient;

	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass) const;
};
