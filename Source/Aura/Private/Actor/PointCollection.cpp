#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

	Pt_00 = CreateDefaultSubobject<USceneComponent>("Pt_00");
	ImmutablePts.Add(Pt_00);
	SetRootComponent(Pt_00);
	Pt_01 = CreateDefaultSubobject<USceneComponent>("Pt_01");
	ImmutablePts.Add(Pt_01);
	Pt_01->SetupAttachment(GetRootComponent());
	Pt_02 = CreateDefaultSubobject<USceneComponent>("Pt_02");
	ImmutablePts.Add(Pt_02);
	Pt_02->SetupAttachment(GetRootComponent());
	Pt_03 = CreateDefaultSubobject<USceneComponent>("Pt_03");
	ImmutablePts.Add(Pt_03);
	Pt_03->SetupAttachment(GetRootComponent());
	Pt_04 = CreateDefaultSubobject<USceneComponent>("Pt_04");
	ImmutablePts.Add(Pt_04);
	Pt_04->SetupAttachment(GetRootComponent());
	Pt_05 = CreateDefaultSubobject<USceneComponent>("Pt_05");
	ImmutablePts.Add(Pt_05);
	Pt_05->SetupAttachment(GetRootComponent());
	Pt_06 = CreateDefaultSubobject<USceneComponent>("Pt_06");
	ImmutablePts.Add(Pt_06);
	Pt_06->SetupAttachment(GetRootComponent());
	Pt_07 = CreateDefaultSubobject<USceneComponent>("Pt_07");
	ImmutablePts.Add(Pt_07);
	Pt_07->SetupAttachment(GetRootComponent());
	Pt_08 = CreateDefaultSubobject<USceneComponent>("Pt_08");
	ImmutablePts.Add(Pt_08);
	Pt_08->SetupAttachment(GetRootComponent());
	Pt_09 = CreateDefaultSubobject<USceneComponent>("Pt_09");
	ImmutablePts.Add(Pt_09);
	Pt_09->SetupAttachment(GetRootComponent());
	Pt_10 = CreateDefaultSubobject<USceneComponent>("Pt_10");
	ImmutablePts.Add(Pt_10);
	Pt_10->SetupAttachment(GetRootComponent());
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& Location, int32 PointNum, float YawOverride)
{
	checkf(ImmutablePts.Num()>=PointNum, TEXT("访问ImmutablePts数组越界"))
	TArray<USceneComponent*> CopyArray;
	for (USceneComponent* Pt : ImmutablePts)
	{
		if (CopyArray.Num() >= PointNum)break;
		if (Pt != Pt_00)
		{
			FVector ToPoint = Pt->GetComponentLocation() - Pt_00->GetComponentLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Pt->SetWorldLocation(Pt_00->GetComponentLocation() + ToPoint);
		}
		const FVector StartPt = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y,
		                                Pt->GetComponentLocation().Z + TraceUpOffset);
		const FVector EndPt = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y,
		                              Pt->GetComponentLocation().Z - TraceDownOffset);
		TArray<AActor*> ActorsToIgnore;
		UAuraAbilitySystemLibrary::GetLivePlayerWithinRadius(this, ActorsToIgnore, TArray<AActor*>(), 1500.f, GetActorLocation());
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActors(ActorsToIgnore);
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByProfile(HitResult, StartPt, EndPt, FName(TEXT("BlockAll")), CollisionQueryParams);
		if (!HitResult.bBlockingHit)continue;
		Pt->SetWorldLocation(HitResult.ImpactPoint);
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));
		CopyArray.Add(Pt);
	}
	return CopyArray;
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
}
