#include "Checkpoint/Checkpoint.h"
#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetupAttachment(CheckpointMesh);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InteractionLocation = CreateDefaultSubobject<USceneComponent>("InteractionLocation");
	InteractionLocation->SetupAttachment(CheckpointMesh);
}

void ACheckpoint::LoadActor_Implementation()
{
	if (bReached)
	{
		HandleGlowEffect();
	}
}

bool ACheckpoint::ShouldSetTransform_Implementation()
{
	return false;
}

void ACheckpoint::GetMoveToLocation_Implementation(FVector& OutLocation)
{
	OutLocation = InteractionLocation->GetComponentLocation();
}

void ACheckpoint::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void ACheckpoint::UnhighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	if (bSaveProgress)
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereBeginOverlap);
	}
}

void ACheckpoint::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                       int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		HandleGlowEffect();
		const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
		FString MapName = GetWorld()->GetMapName();
		MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
		AuraGameMode->SaveWorldState(GetWorld(), MapName);
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
	}
}

void ACheckpoint::HandleGlowEffect()
{
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstance);
	CheckpointReached(DynamicMaterialInstance);
}
