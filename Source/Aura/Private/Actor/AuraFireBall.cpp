#include "Actor/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/AuraLogChannel.h"
#include "Components/AudioComponent.h"

AAuraFireBall::AAuraFireBall()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AAuraFireBall::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetInstigator() == OtherActor || !UAuraAbilitySystemLibrary::IsNotFriend(GetInstigator(), OtherActor) || !HasAuthority())return;
	//GE的应用仅在服务器端执行，客户端属性会同步过去
	if (UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		DamageEffectParams.TargetGameplayAbilitySystem = TargetAsc;
		DamageEffectParams.DeathImpulse = DamageEffectParams.DeathImpulseMagnitude * GetActorForwardVector();
		UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
	}
}

void AAuraFireBall::CauseExplosionDamage()
{
	ExplosionEffectParams.bIsRadialDamage = true;
	ExplosionEffectParams.RadialDamageInnerRadius = ExplosionInnerRadius;
	ExplosionEffectParams.RadialDamageOuterRadius = ExplosionOuterRadius;
	ExplosionEffectParams.RadialDamageOrigin = GetActorLocation();
	TArray<AActor*> ActorsToIgnore = {GetInstigator()};
	TArray<AActor*> ExplodedActors;
	UAuraAbilitySystemLibrary::GetLivePlayerWithinRadius(this, ExplodedActors, ActorsToIgnore, ExplosionOuterRadius, GetActorLocation());
	for (AActor* Actor : ExplodedActors)
	{
		if (!UAuraAbilitySystemLibrary::IsNotFriend(Actor, GetInstigator()))continue;
		if (UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
		{
			ExplosionEffectParams.TargetGameplayAbilitySystem = TargetAsc;
			FRotator KnockBackRotation = (Actor->GetActorLocation() - GetActorLocation()).GetSafeNormal().Rotation();
			KnockBackRotation.Pitch = 45.f;
			ExplosionEffectParams.DeathImpulse = KnockBackRotation.Vector() * DeathImpulseMagnitude;
			ExplosionEffectParams.KnockBackForce = KnockBackMagnitude * KnockBackRotation.Vector();
			UAuraAbilitySystemLibrary::ApplyDamageEffect(ExplosionEffectParams);
		}
	}
}

