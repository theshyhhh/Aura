#include "Input/AuraInputComponent.h"

UAuraInputComponent::UAuraInputComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAuraInputComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAuraInputComponent::BeginPlay()
{
	Super::BeginPlay();
}
