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
	virtual int32 GetCharacterLevel_Implementation() const override;
	//end ICombatInterface

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	//PlayerInterface Begin
	virtual void AddXP_Implementation(const int32 InXP) override;

	virtual void LevelUp_Implementation() override;

	virtual int32 GetXP_Implementation() const override;

	virtual int32 FindLevelByXP_Implementation(const int32 XP) override;
	//PlayerInterfaceEnd

private:
	virtual void InitAbilityActorInfo() override;
};
