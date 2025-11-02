#include "AbilitySystem/Abilities/AuraSummonAbility.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const FVector Forward = AvatarActor->GetActorForwardVector();
	const FVector Location = AvatarActor->GetActorLocation();
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	const float DeltaSpread = SpawnSpread / NumMinions;
	TArray<FVector> SpawnLocations;
	for (int i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		const float Distance = FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		FVector SpawnLocation = Location + Direction * Distance;
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, SpawnLocation + FVector(0, 0, 500.f), SpawnLocation + FVector(0, 0, -500), ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			SpawnLocation = Hit.ImpactPoint;
		}
		SpawnLocations.Add(SpawnLocation);
	}
	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}
