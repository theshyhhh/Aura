#pragma once

#include "CoreMinimal.h"
#include "AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class ULoadScreenSaveGame;
class UCameraComponent;
class USpringArmComponent;
class UNiagaraComponent;

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

	virtual int32 GetAttributePointReward_Implementation(const int32 Level) const override;

	virtual int32 GetSpellPointReward_Implementation(const int32 Level) const override;

	virtual void AddPlayerLevel_Implementation(const int32 InLevel) override;

	virtual void AddAttributePoint_Implementation(const int32 InAttributePoint) override;

	virtual void AddSpellPoint_Implementation(const int32 InSpellPoint) override;

	virtual int32 GetPlayerAttributePoint_Implementation() const override;

	virtual int32 GetPlayerSpellPoint_Implementation() const override;

	virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial = nullptr) override;

	virtual void HideMagicCircle_Implementation() override;

	virtual void SaveProgress_Implementation(const FName& CheckpointTag) override;
	//PlayerInterfaceEnd

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

private:
	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticle() const;

	void LoadProgress();

	void InitPrimaryAttributeFromSaveGame(const ULoadScreenSaveGame* SaveGame);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> PrimaryAttributeEffect_SetByCaller;
};
