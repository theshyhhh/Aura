#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32); //用于广播玩家数据变化的委托
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLevelChangedSignature, int32/*NewLevel*/, bool/*是否播放升级特效*/);
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	FORCEINLINE UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }

	FORCEINLINE int32 GetPlayerXP() const { return XP; }

	FORCEINLINE int32 GetAttributePoint() const { return AttributePoint; }

	FORCEINLINE int32 GetSpellPoint() const { return SpellPoint; }

	void AddXP(const int32 XPToAdd);

	void AddLevel(const int32 LevelToAdd);

	void AddAttributePoint(const int32 AttributePointToAdd);

	void AddSpellPoint(const int32 SpellPointToAdd);

	void SetXP(const int32 NewXP);

	void SetLevel(const int32 NewLevel);

	void SetAttributePoint(const int32 InAttributePoint);

	void SetSpellPoint(const int32 InSpellPoint);

	FOnPlayerStatChangedSignature OnXPChangedDelegate;

	FOnLevelChangedSignature OnLevelChangedDelegate;

	FOnPlayerStatChangedSignature OnAttributePointChangedDelegate;

	FOnPlayerStatChangedSignature OnSpellPointChangedDelegate;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoint)
	int32 AttributePoint = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoint)
	int32 SpellPoint = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel) const;

	UFUNCTION()
	void OnRep_XP(int32 OldXP) const;

	UFUNCTION()
	void OnRep_AttributePoint(int32 OldAttributePoint) const;

	UFUNCTION()
	void OnRep_SpellPoint(int32 OldSpellPoint) const;
};
