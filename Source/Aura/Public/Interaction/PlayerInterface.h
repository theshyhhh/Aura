#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UINTERFACE()
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void AddXP(const int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelByXP(const int32 XP);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointReward(const int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointReward(const int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	void AddPlayerLevel(const int32 InLevel);

	UFUNCTION(BlueprintNativeEvent)
	void AddAttributePoint(const int32 InAttributePoint);

	UFUNCTION(BlueprintNativeEvent)
	void AddSpellPoint(const int32 InSpellPoint);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetPlayerAttributePoint() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetPlayerSpellPoint() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideMagicCircle();
};
