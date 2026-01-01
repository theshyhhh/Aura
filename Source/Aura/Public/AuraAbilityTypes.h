#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams() = default;

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceGameplayAbilitySystem = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetGameplayAbilitySystem = nullptr;

	UPROPERTY()
	float BaseDamage = 0.0f;

	UPROPERTY()
	float AbilityLevel = 1.0f;

	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY()
	float DebuffChance = 0.f;

	UPROPERTY()
	float DebuffDamage = 0.f;

	UPROPERTY()
	float DebuffFrequency = 0.f;

	UPROPERTY()
	float DebuffDuration = 0.f;
};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	bool IsBlockedHit() const { return bIsBlockedHit; }

	bool IsCriticalHit() const { return bIsCriticalHit; }

	bool IsDebuffAppliedSuccessfully() const { return bDebuffAppliedSuccessfully; }

	float GetDebuffDamage() const { return DebuffDamage; }

	float GetDebuffDuration() const { return DebuffDuration; }

	float GetDebuffFrequency() const { return DebuffFrequency; }

	FGameplayTag GetDamageType() const { return *DamageType; }

	void SetIsBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }

	void SetIsCriticalHit(const bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }

	void SetDebuffAppliedSuccessfully(const bool bInDebuffAppliedSuccessfully) { bDebuffAppliedSuccessfully = bInDebuffAppliedSuccessfully; }

	void SetDebuffDamage(const float InDebuffDamage) { DebuffDamage = InDebuffDamage; }

	void SetDebuffDuration(const float InDebuffDuration) { DebuffDuration = InDebuffDuration; }

	void SetDebuffFrequency(const float InDebuffFrequency) { DebuffFrequency = InDebuffFrequency; }

	void SetDamageType(FGameplayTag InDamageType) { DamageType = MakeShared<FGameplayTag>(InDamageType); }


	//必须重载
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	//数据网络序列化
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

protected:
	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	bool bDebuffAppliedSuccessfully = false;

	UPROPERTY()
	float DebuffDamage = 0.f;

	UPROPERTY()
	float DebuffDuration = 0.f;

	UPROPERTY()
	float DebuffFrequency = 0.f;

	TSharedPtr<FGameplayTag> DamageType;
};

template <>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true // Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};
