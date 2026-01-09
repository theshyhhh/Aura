#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "GameFramework/Character.h"

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
