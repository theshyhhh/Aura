#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD()))
		{
			AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
			UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();
			return AuraHUD->GetOverlayWidgetController(FAuraWidgetControllerParams(PlayerController, PlayerState, ASC, AttributeSet));
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD()))
		{
			AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
			UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();
			return AuraHUD->GetAttributeMenuWidgetController(FAuraWidgetControllerParams(PlayerController, PlayerState, ASC, AttributeSet));
		}
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
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor()))
		{
			FGameplayAbilitySpec AbilitySpec(Ability, CombatInterface->GetCharacterLevel());
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
