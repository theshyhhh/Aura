#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAbilityInfo;
class AAuraHUD;
struct FAuraWidgetControllerParams;
class USpellMenuWidgetController;
struct FGameplayEffectContextHandle;
class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;

UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool MakeAuraWidgetControllerParams(const UObject* WorldContextObject, FAuraWidgetControllerParams& Params, AAuraHUD*& AuraHUD);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Character Class Defaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level,
	                                        UAbilitySystemComponent* Asc);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Character Class Defaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Character Class Defaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Ability")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsDebuffAppliedSuccessfully(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FVector GetKnockBackForce(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Gameplay Mechanics")
	static void GetLivePlayerWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors,
	                                      const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	/*
	 *如果不是友军返回true
	 *如果是，返回false
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraAbilitySystemLibrary|Gameplay Mechanics")
	static bool IsNotFriend(const AActor* FirstActor, const AActor* SecondActor);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& Params);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraAbilitySystemLibrary|Gameplay Mechanics")
	static TArray<FRotator> GetEvenlyRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 Num);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraAbilitySystemLibrary|Gameplay Mechanics")
	static TArray<FVector> GetEvenlyVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 Num);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Gameplay Mechanics")
	static void GetClosestTargets(int32 MaxTargets, TArray<AActor*> Actors, TArray<AActor*>& OutClosestActors, const FVector& Origin);

	/** 此函数为计算范围性伤害，可以根据距离和障碍物进行精准控制最终造成的伤害
 * @param TargetActor - 需要计算攻击的目标
 * @param BaseDamage - 在伤害内半径（DamageInnerRadius）内应用的最大伤害值。
 * @param MinimumDamage - 在伤害外半径（DamageOuterRadius）处应用的最小伤害值。如果为0将不受伤害
 * @param Origin - 爆炸的原点（中心位置），即伤害的起点。
 * @param DamageInnerRadius - 全伤害半径：在该范围内的所有对象会受到最大伤害（BaseDamage）。
 * @param DamageOuterRadius - 最小伤害半径：在该范围之外的对象只会受到**MinimumDamage**。
 * @param DamageFalloff - 控制伤害递减的速率。值越高，伤害递减得越快。
 * @param DamageCauser - 伤害的直接来源，如爆炸的手雷或火箭弹。
 * @param InstigatedByController - 造成伤害的控制器，通常是执行该行为的玩家控制器。
 * @param DamagePreventionChannel - 阻挡伤害的通道。如果某个对象阻挡了该通道上的检测，则不会对目标应用伤害（如墙壁阻挡了视线）。
 * @return 返回对目标计算后的范围攻击应造成的伤害
 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="AuraAbilitySystemLibrary|GameplayMechanics",
		meta=(WorldContext="WorldContextObject", AutoCreateRefTerm="IgnoreActors"))
	static float ApplyRadialDamageWithFalloff(AActor* TargetActor, float BaseDamage, float MinimumDamage, const FVector& Origin,
	                                          float DamageInnerRadius, float DamageOuterRadius,
	                                          float DamageFalloff, AActor* DamageCauser = nullptr, AController* InstigatedByController = nullptr,
	                                          ECollisionChannel DamagePreventionChannel = ECC_Visibility);


	static int32 GetXPRewardByClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 Level);
};
