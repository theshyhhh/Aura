#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams() = default;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceGameplayAbilitySystem = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetGameplayAbilitySystem = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	float KnockBackChance = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float KnockBackMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	FVector KnockBackForce = FVector::ZeroVector;
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

	FVector GetDeathImpulse() const { return DeathImpulse; }

	FVector GetKnockBackForce() const { return KnockBackForce; }

	void SetIsBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }

	void SetIsCriticalHit(const bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }

	void SetDebuffAppliedSuccessfully(const bool bInDebuffAppliedSuccessfully) { bDebuffAppliedSuccessfully = bInDebuffAppliedSuccessfully; }

	void SetDebuffDamage(const float InDebuffDamage) { DebuffDamage = InDebuffDamage; }

	void SetDebuffDuration(const float InDebuffDuration) { DebuffDuration = InDebuffDuration; }

	void SetDebuffFrequency(const float InDebuffFrequency) { DebuffFrequency = InDebuffFrequency; }

	void SetDamageType(FGameplayTag InDamageType) { DamageType = MakeShared<FGameplayTag>(InDamageType); }

	void SetDeathImpulse(const FVector& InImpulse) { DeathImpulse = InImpulse; }

	void SetKnockBackForce(const FVector& InForce) { KnockBackForce = InForce; }


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

	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;

	UPROPERTY()
	FVector KnockBackForce = FVector::ZeroVector;
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
