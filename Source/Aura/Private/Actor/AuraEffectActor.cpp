#include "Actor/AuraEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
}

void AAuraEffectActor::RotateAndSineMove(float DeltaSeconds)
{
	if (bEnableRotation)
	{
		AddActorWorldRotation(FRotator(0.0f, DeltaSeconds * RotateSpeed, 0.0f));
	}
	if (bEnableSineMovement)
	{
		SetActorLocation(InitialLocation + FVector(0, 0, FMath::Sin(SineRuntime * SinePeriodConstant)) * SineAmplitude);
		SineRuntime += DeltaSeconds;
		if (SineRuntime >= PI * 2)SineRuntime = 0;
	}
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)return;
	check(GameplayEffectClass)
	//生成GE上下文
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle GameplayEffectSpecHandle = TargetASC->MakeOutgoingSpec(
		GameplayEffectClass, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(
		*GameplayEffectSpecHandle.Data.Get());
	const bool bIsInfinite = GameplayEffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy ==
		EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy != EEffectRemovalPolicy::DoNotRemove)
	{
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetASC);
	}
	if (bDestroyOnEffectApplication)Destroy();
}

void AAuraEffectActor::OnBeginOverlap(AActor* TargetActor)
{
	if (!bApplyToEnemy && TargetActor->ActorHasTag(FName("Enemy")))return;
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (!bApplyToEnemy && TargetActor->ActorHasTag(FName("Enemy")))return;
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC))return;
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (auto it : ActiveEffectHandles)
		{
			if (it.Value == TargetASC)
			{
				TargetASC->RemoveActiveGameplayEffect(it.Key, 1);
				HandlesToRemove.Add(it.Key);
			}
		}
		for (auto& it : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(it); //从Map中移除
		}
	}
}
