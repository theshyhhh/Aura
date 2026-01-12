#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::SetMouseHitInfo(const FHitResult& MouseHitResult)
{
	if (MouseHitResult.bBlockingHit)
	{
		MouseHitLocation = MouseHitResult.ImpactPoint;
		HitActor = MouseHitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

APlayerController* UAuraBeamSpell::GetOwnerController() const
{
	return CurrentActorInfo->PlayerController.Get();
}

ACharacter* UAuraBeamSpell::GetOwnerCharacter() const
{
	return Cast<ACharacter>(CurrentActorInfo->AvatarActor);
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& StartLoc, const FVector& EndLoc)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwnerCharacter());
	FHitResult HitResult;
	UKismetSystemLibrary::SphereTraceSingle(GetOwnerCharacter(),
	                                        StartLoc,
	                                        EndLoc,
	                                        10.f,
	                                        TraceTypeQuery1,
	                                        false,
	                                        ActorsToIgnore,
	                                        EDrawDebugTrace::None,
	                                        HitResult,
	                                        true);
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		HitActor = HitResult.GetActor();
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(HitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::OnPrimaryTargetDeath))
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::OnPrimaryTargetDeath);
	}
}

void UAuraBeamSpell::StoreAdditionalTarget(TArray<AActor*>& OutAdditionalActors)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(HitActor.Get());
	TArray<AActor*> OverlappingActors;
	UAuraAbilitySystemLibrary::GetLivePlayerWithinRadius(GetAvatarActorFromActorInfo(),
	                                                     OverlappingActors,
	                                                     ActorsToIgnore,
	                                                     850.f,
	                                                     HitActor.Get()->GetActorLocation());
	int32 ShockTargetNum = FMath::Min(MaxShockTargetNum, GetAbilityLevel());
	TArray<AActor*> Friends;
	for (AActor* Actor : OverlappingActors)
	{
		if (!UAuraAbilitySystemLibrary::IsNotFriend(GetAvatarActorFromActorInfo(), Actor))
		{
			Friends.Add(Actor);
		}
	}
	for (AActor* Actor : Friends)
	{
		OverlappingActors.Remove(Actor);
	}
	UAuraAbilitySystemLibrary::GetClosestTargets(ShockTargetNum, OverlappingActors, OutAdditionalActors, HitActor.Get()->GetActorLocation());
	for (AActor* Actor : OutAdditionalActors)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Actor))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::OnAdditionalTargetDeath))
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::OnAdditionalTargetDeath);
		}
	}
}
