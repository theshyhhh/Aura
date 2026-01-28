#include "Actor/MagicCircle.h"
#include "Components/DecalComponent.h"

AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
}
