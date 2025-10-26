// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include <Programs/UnrealBuildAccelerator/Core/Public/UbaBase.h>

#include "Kismet/KismetSystemLibrary.h"

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
		UKismetSystemLibrary::DrawDebugArrow(AvatarActor, Location, Location + Direction * MaxSpawnDistance, 10.f, FLinearColor::Red, 10);
		const float Distance = FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		const FVector SpawnLocation = Location + Direction * Distance;
		SpawnLocations.Add(SpawnLocation);
		UKismetSystemLibrary::DrawDebugSphere(AvatarActor, SpawnLocation, 30, 10, FLinearColor::Green, 10);
	}
	return SpawnLocations;
}
