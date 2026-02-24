#include "Actor/AuraEnemySpawnVolume.h"

#include "Actor/AuraEnemySpawnPoint.h"
#include "Components/BoxComponent.h"
#include "Interaction/PlayerInterface.h"

AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	SetRootComponent(BoxComponent);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionObjectType(ECC_WorldStatic);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAuraEnemySpawnVolume::LoadActor_Implementation()
{
	if (bReached)
		Destroy();
}

bool AAuraEnemySpawnVolume::ShouldSetTransform_Implementation()
{
	return false;
}

void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("生成区域加载"))
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraEnemySpawnVolume::OnBoxBeginOverlap);
}

void AAuraEnemySpawnVolume::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                              int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UPlayerInterface>())return;
	bReached = true;
	for (const AAuraEnemySpawnPoint* Point : SpawnPoints)
	{
		if (IsValid(Point))
		{
			Point->SpawnEnemy();
		}
	}
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
