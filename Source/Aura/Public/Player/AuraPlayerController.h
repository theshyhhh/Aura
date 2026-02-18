#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class AMagicCircle;
class UNiagaraSystem;
class UDamageTextComponent;
class UAuraAbilitySystemComponent;
class UAuraInputConfig;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class USplineComponent;

enum class ETargetingStatus: uint8
{
	TargetingEnemy,
	TargetingNonEnemy,
	TargetingNone
};

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	//在目标头顶上显示伤害数字，服务端调用，客户端执行
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(const float DamageAmount, ACharacter* TargetCharacter, const bool bIsBlockedHit, const bool bIsCriticalHit);

	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	//移动
	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();

	//鼠标位置射线检测到的上一个Actor
	TWeakObjectPtr<AActor> LastActor;

	//鼠标位置射线检测到的当前Actor
	TWeakObjectPtr<AActor> CurrentActor;

	/*
	 * 检测输入触发能力相关
	 */
	void AbilityInputTagPressed(FGameplayTag Tag);
	void AbilityInputTagReleased(FGameplayTag Tag);
	void AbilityInputTagHeld(FGameplayTag Tag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	/*
	 * 鼠标点击移动相关
	 */

	/*是否鼠标正指向敌人*/
	ETargetingStatus TargetingStatus = ETargetingStatus::TargetingNone;

	/*鼠标按下时检测到的位置*/
	FVector CachedDestination = FVector::ZeroVector;

	/*鼠标持续按下的时间*/
	float FollowTime = 0.f;

	/*视为鼠标短按的阈值*/
	float ShortPressThreshold = 0.5f;

	/*是否在自动移动的标志*/
	bool bAutoRunning = false;

	/*自动移动时，距离目标地点停止的可接受半径*/
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	/*自动移动时，使路线更加平滑的样条线*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> SplineComponent;

	/*沿样条线自动移动*/
	void AutoRun();

	//鼠标射线检测碰撞信息
	FHitResult CursorHitResult;

	//是否按下shift
	bool bShiftPressed = false;
	void ShiftPressed() { bShiftPressed = true; }
	void ShiftReleased() { bShiftPressed = false; }

	//伤害数字组件类
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;

	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircle;

	void UpdateMagicCircle();
};
