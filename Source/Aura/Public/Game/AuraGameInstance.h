#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString LoadSlotName = FString();

	UPROPERTY()
	FName PlayerStartTag = FName();

	UPROPERTY()
	int32 LoadSlotIndex;
};
