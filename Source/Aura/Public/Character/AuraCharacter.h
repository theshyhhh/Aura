#pragma once

#include "CoreMinimal.h"
#include "AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();

	//begin ICombatInterface
	virtual int32 GetCharacterLevel() const override;
	//end ICombatInterface

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	//PlayerInterface Begin
	virtual void AddXP_Implementation(int32 InXP) override;
	//PlayerInterfaceEnd

private:
	virtual void InitAbilityActorInfo() override;
};
