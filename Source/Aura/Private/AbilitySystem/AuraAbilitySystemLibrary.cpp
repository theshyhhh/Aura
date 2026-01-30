#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectTypes.h"
#include "Engine/DamageEvents.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

bool UAuraAbilitySystemLibrary::MakeAuraWidgetControllerParams(const UObject* WorldContextObject, FAuraWidgetControllerParams& Params,
                                                               AAuraHUD*& AuraHUD)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		Params.PlayerController = PlayerController;
		AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD());
		if (AuraHUD)
		{
			AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
			UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();
			Params.AttributeSet = AttributeSet;
			Params.AbilitySystemComponent = ASC;
			Params.PlayerState = PlayerState;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	AAuraHUD* AuraHUD = nullptr;
	FAuraWidgetControllerParams Params;
	if (MakeAuraWidgetControllerParams(WorldContextObject, Params, AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(Params);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	AAuraHUD* AuraHUD = nullptr;
	FAuraWidgetControllerParams Params;
	if (MakeAuraWidgetControllerParams(WorldContextObject, Params, AuraHUD))
	{
		return AuraHUD->GetAttributeMenuWidgetController(Params);
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	AAuraHUD* AuraHUD = nullptr;
	FAuraWidgetControllerParams Params;
	if (MakeAuraWidgetControllerParams(WorldContextObject, Params, AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(Params);
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, const ECharacterClass CharacterClass,
                                                            const float Level,
                                                            UAbilitySystemComponent* Asc)
{
	const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo)return;
	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	FGameplayEffectContextHandle ContextHandle = Asc->MakeEffectContext();
	ContextHandle.AddSourceObject(Asc->GetAvatarActor());
	const FGameplayEffectSpecHandle PrimaryAttributeSpecHandle = Asc->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level,
	                                                                                   ContextHandle);
	Asc->ApplyGameplayEffectSpecToSelf(*PrimaryAttributeSpecHandle.Data.Get());
	const FGameplayEffectSpecHandle SecondaryAttributeSpecHandle = Asc->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level,
	                                                                                     ContextHandle);
	Asc->ApplyGameplayEffectSpecToSelf(*SecondaryAttributeSpecHandle.Data.Get());
	const FGameplayEffectSpecHandle VitalAttributeSpecHandle = Asc->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level,
	                                                                                 ContextHandle);
	Asc->ApplyGameplayEffectSpecToSelf(*VitalAttributeSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	const UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo)return;
	for (const auto AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo& ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (auto Ability : ClassDefaultInfo.StartupAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec(Ability, ICombatInterface::Execute_GetCharacterLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!AuraGameMode)return nullptr;
	return AuraGameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!AuraGameMode)return nullptr;
	return AuraGameMode->AbilityInfo;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return EffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return EffectContext->IsCriticalHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsDebuffAppliedSuccessfully(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return EffectContext->IsDebuffAppliedSuccessfully();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return EffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return EffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return EffectContext->GetDebuffDuration();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		if (EffectContext->GetDamageType().IsValid())
			return EffectContext->GetDamageType();
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return EffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockBackForce(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return EffectContext->GetKnockBackForce();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::GetLivePlayerWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors,
                                                          const TArray<AActor*>& ActorsToIgnore, const float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParam;
	SphereParam.AddIgnoredActors(ActorsToIgnore);

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity,
		                                FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
		                                FCollisionShape::MakeSphere(Radius), SphereParam);
		for (const FOverlapResult& OverlapResult : Overlaps)
		{
			AActor* OverlapActor = OverlapResult.GetActor();
			if (OverlapActor->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(OverlapActor))
			{
				OutOverlappingActors.AddUnique(OverlapActor);
			}
		}
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(const AActor* FirstActor, const AActor* SecondActor)
{
	const bool bBothPlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	return !(bBothPlayers || bBothEnemies);
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& Params)
{
	FGameplayEffectContextHandle ContextHandle = Params.SourceGameplayAbilitySystem->MakeEffectContext();
	const AActor* SourceAvatarActor = Params.SourceGameplayAbilitySystem->GetAvatarActor();
	ContextHandle.AddSourceObject(SourceAvatarActor);
	if (FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraGameplayEffectContext->SetDeathImpulse(Params.DeathImpulse);
		AuraGameplayEffectContext->SetKnockBackForce(Params.KnockBackForce);
		if (Params.bIsRadialDamage)
		{
			AuraGameplayEffectContext->SetIsRadialDamage(true);
			AuraGameplayEffectContext->SetRadialDamageInnerRadius(Params.RadialDamageInnerRadius);
			AuraGameplayEffectContext->SetRadialDamageOuterRadius(Params.RadialDamageOuterRadius);
			AuraGameplayEffectContext->SetRadialDamageOrigin(Params.RadialDamageOrigin);
		}
	}
	FGameplayEffectSpecHandle SpecHandle = Params.SourceGameplayAbilitySystem->MakeOutgoingSpec(
		Params.DamageGameplayEffectClass, Params.AbilityLevel, ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Params.DamageType, Params.BaseDamage);
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Debuff_Chance, Params.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Debuff_Duration, Params.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Debuff_Frequency, Params.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Debuff_Damage, Params.DebuffDamage);
	Params.SourceGameplayAbilitySystem->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, Params.TargetGameplayAbilitySystem);
	return ContextHandle;
}

/*
 * 获得以Forward为中心，Axis为轴，在Spread角度内数量为Num平均分布的方向
 */
TArray<FRotator> UAuraAbilitySystemLibrary::GetEvenlyRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 Num)
{
	TArray<FRotator> Rotators;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2, Axis);
	if (Num > 1)
	{
		float DeltaAngle = Spread / (Num - 1);
		for (int i = 0; i < Num; i++)
		{
			Rotators.Add(LeftOfSpread.RotateAngleAxis(DeltaAngle * i, Axis).Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

/*
 * 获得以Forward为中心，Axis为轴，在Spread角度内数量为Num平均分布的方向
 */
TArray<FVector> UAuraAbilitySystemLibrary::GetEvenlyVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 Num)
{
	TArray<FVector> Vectors;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2, Axis);
	if (Num > 1)
	{
		float DeltaAngle = Spread / (Num - 1);
		for (int i = 0; i < Num; i++)
		{
			Vectors.Add(LeftOfSpread.RotateAngleAxis(DeltaAngle * i, Axis));
		}
	}
	else
	{
		Vectors.Add(Forward);
	}
	return Vectors;
}

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, TArray<AActor*> Actors, TArray<AActor*>& OutClosestActors,
                                                  const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)
	{
		OutClosestActors = Actors;
		return;
	}
	int32 FoundTargetsNum = 0;
	while (FoundTargetsNum < MaxTargets)
	{
		double MinDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor = nullptr;
		for (AActor* Actor : Actors)
		{
			double Distance = FVector::Distance(Origin, Actor->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestActor = Actor;
			}
		}
		Actors.Remove(ClosestActor);
		OutClosestActors.AddUnique(ClosestActor);
		FoundTargetsNum++;
	}
}

/** @RETURN True if weapon trace from Origin hits component VictimComp.  OutHitResult will contain properties of the hit. */
static bool ComponentIsDamageableFrom(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor,
                                      const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult)
{
	FCollisionQueryParams LineParams(SCENE_QUERY_STAT(ComponentIsVisibleFrom), true, IgnoredActor);
	LineParams.AddIgnoredActors(IgnoreActors);

	// Do a trace from origin to middle of box
	UWorld* const World = VictimComp->GetWorld();
	check(World);

	FVector const TraceEnd = VictimComp->Bounds.Origin;
	FVector TraceStart = Origin;
	if (Origin == TraceEnd)
	{
		// tiny nudge so LineTraceSingle doesn't early out with no hits
		TraceStart.Z += 0.01f;
	}

	// Only do a line trace if there is a valid channel, if it is invalid then result will have no fall off
	if (TraceChannel != ECollisionChannel::ECC_MAX)
	{
		bool const bHadBlockingHit = World->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, TraceChannel, LineParams);
		//::DrawDebugLine(World, TraceStart, TraceEnd, FLinearColor::Red, true);

		// If there was a blocking hit, it will be the last one
		if (bHadBlockingHit)
		{
			if (OutHitResult.Component == VictimComp)
			{
				// if blocking hit was the victim component, it is visible
				return true;
			}
			else
			{
				// if we hit something else blocking, it's not
				UE_LOG(LogDamage, Log, TEXT("Radial Damage to %s blocked by %s (%s)"), *GetNameSafe(VictimComp),
				       *OutHitResult.GetHitObjectHandle().GetName(), *GetNameSafe(OutHitResult.Component.Get()));
				return false;
			}
		}
	}
	else
	{
		UE_LOG(LogDamage, Warning, TEXT("ECollisionChannel::ECC_MAX is not valid! No falloff is added to damage"));
	}

	// didn't hit anything, assume nothing blocking the damage and victim is consequently visible
	// but since we don't have a hit result to pass back, construct a simple one, modeling the damage as having hit a point at the component's center.
	FVector const FakeHitLoc = VictimComp->GetComponentLocation();
	FVector const FakeHitNorm = (Origin - FakeHitLoc).GetSafeNormal(); // normal points back toward the epicenter
	OutHitResult = FHitResult(VictimComp->GetOwner(), VictimComp, FakeHitLoc, FakeHitNorm);
	return true;
}

float UAuraAbilitySystemLibrary::ApplyRadialDamageWithFalloff(AActor* TargetActor, float BaseDamage, float MinimumDamage, const FVector& Origin,
                                                              float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff,
                                                              AActor* DamageCauser, AController* InstigatedByController,
                                                              ECollisionChannel DamagePreventionChannel)
{
	// 判断目标角色是否死亡
	bool bIsDead = true;
	if (TargetActor->Implements<UCombatInterface>())
	{
		bIsDead = ICombatInterface::Execute_IsDead(TargetActor);
	}
	if (bIsDead)
	{
		return 0.f; //如果角色已经死亡，直接返回0
	}

	// 获取目标角色所有组件
	TArray<UActorComponent*> Components;
	TargetActor->GetComponents(Components);

	bool bIsDamageable = false; //判断攻击是能能够查看到目标
	TArray<FHitResult> HitList; //存储目标收到碰撞查询到的碰撞结果
	for (UActorComponent* Comp : Components)
	{
		UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Comp);
		if (PrimitiveComp && PrimitiveComp->IsCollisionEnabled())
		{
			FHitResult Hit;
			bIsDamageable = ComponentIsDamageableFrom(
				PrimitiveComp, Origin, DamageCauser, {DamageCauser}, DamagePreventionChannel, Hit
			);
			HitList.Add(Hit);
			if (bIsDamageable) break;
		}
	}

	//应用目标的伤害值
	float AppliedDamage = 0.f;

	if (bIsDamageable)
	{
		// 创建伤害事件
		FRadialDamageEvent DmgEvent;
		DmgEvent.DamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		DmgEvent.Origin = Origin;
		DmgEvent.Params = FRadialDamageParams(BaseDamage, MinimumDamage, DamageInnerRadius, DamageOuterRadius, DamageFalloff);
		DmgEvent.ComponentHits = HitList;

		// 应用伤害
		AppliedDamage = TargetActor->TakeDamage(BaseDamage, DmgEvent, InstigatedByController, DamageCauser);
	}

	return AppliedDamage;
}

int32 UAuraAbilitySystemLibrary::GetXPRewardByClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 Level)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo)return -1;
	const int32 XPReward = static_cast<int32>(CharacterClassInfo->GetClassDefaultInfo(CharacterClass).XPReward.GetValueAtLevel(Level));
	return XPReward;
}
