#include "Player/AuraPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Aura/Aura.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	//可被复制，用于网络复制
	bReplicates = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
	UpdateMagicCircle();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(const float DamageAmount, ACharacter* TargetCharacter, const bool bIsBlockedHit,
                                                            const bool bIsCriticalHit)
{
	check(DamageTextComponentClass)
	if (IsValid(TargetCharacter))
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bIsBlockedHit, bIsCriticalHit);
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//断言：检测AuraContext是否已设置，如果没有设置，则强制停止执行
	check(AuraContext);
	//通过本地玩家的增强输入子系统，将InputMappingContext添加给本地玩家，并且可以设置优先级
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;
	//配置鼠标光标类型
	DefaultMouseCursor = EMouseCursor::Default;

	//配置一些输入的模式
	FInputModeGameAndUI InputModeData;
	//是否将鼠标锁定到视口中
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	//设置在鼠标被临时捕获（如按下鼠标拖拽时）时，是否隐藏鼠标指针。
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(MoveAction);
	//InputComponent实际上指向的是一个UEnhancedInputComponent，CastChecked相当于Cast+check组合
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &AAuraPlayerController::AbilityInputTagPressed,
	                                       &AAuraPlayerController::AbilityInputTagReleased, &AAuraPlayerController::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (AuraAbilitySystemComponent && AuraAbilitySystemComponent->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	//获得控制器只计算Yaw旋转的前向向量和右向向量
	const FVector2D InputAxisValue = InputActionValue.Get<FVector2D>();
	const FRotator ControllerYaw = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	const FVector ForwardVector = FRotationMatrix(ControllerYaw).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(ControllerYaw).GetUnitAxis(EAxis::Y);

	//移动输入
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardVector, InputAxisValue.Y);
		ControlledPawn->AddMovementInput(RightVector, InputAxisValue.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	if (AuraAbilitySystemComponent && AuraAbilitySystemComponent->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		return;
	}
	GetHitResultUnderCursor(IsValid(MagicCircle) ? ECC_ExcludePawn : ECC_Visibility, false, CursorHitResult);
	if (!CursorHitResult.bBlockingHit)return;

	LastActor = CurrentActor;
	AActor* HitActor = CursorHitResult.GetActor();
	if (HitActor && HitActor->Implements<UEnemyInterface>())
	{
		CurrentActor = HitActor;
	}
	else
	{
		CurrentActor = nullptr;
	}
	/*
	 *当LastActor和CurrentActor都为空，或都不为空且相同时，什么都不做
	 *当LastActor为空,CurrentActor不为空时，CurrentActor调用HighlightActor
	 *当CurrentActor为空，LastActor不为空时，LastActor调用UnHighlightActor
	 *当都不为空时，且不相同，LastActor调用UnHighlightActor，CurrentActor调用HighlightActor
	 */
	if (LastActor != CurrentActor)
	{
		if (LastActor.IsValid())
		{
			IEnemyInterface* LastPtr = Cast<IEnemyInterface>(LastActor.Get());
			LastPtr->UnHighlightActor();
		}
		if (CurrentActor.IsValid())
		{
			IEnemyInterface* CurrentPtr = Cast<IEnemyInterface>(CurrentActor.Get());
			CurrentPtr->HighlightActor();
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag Tag)
{
	if (AuraAbilitySystemComponent && AuraAbilitySystemComponent->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	//按下右键负责移动和激活能力，通过鼠标是否有指向的Actor选择攻击或移动
	if (Tag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		bTargeting = CurrentActor.IsValid(); //检测当前鼠标是否有指向Actor
		bAutoRunning = false;
	}
	if (AuraAbilitySystemComponent)AuraAbilitySystemComponent->AbilityInputTagPressed(Tag);
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag Tag)
{
	if (AuraAbilitySystemComponent && AuraAbilitySystemComponent->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}
	if (!Tag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		if (GetAuraAbilitySystemComponent())GetAuraAbilitySystemComponent()->AbilityInputTagReleased(Tag);
		return;
	}
	if (GetAuraAbilitySystemComponent())GetAuraAbilitySystemComponent()->AbilityInputTagReleased(Tag);
	//既没有瞄准的目标也没有按下shift时，如果右键按下时间小于最短按下时间，则进行自动寻路
	if (!bTargeting && !bShiftPressed)
	{
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
				this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				SplineComponent->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					SplineComponent->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				}
				if (NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints.Last(); //将目标地点设置为最后一个导航点，防止点击的目标地点无法正确到达导致的一直在自动移动
					bAutoRunning = true;
				}
			}
			if (AuraAbilitySystemComponent && !AuraAbilitySystemComponent->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			}
		}
	}
	FollowTime = 0.f;
}

/*
 *持续按下时，先判断是否是右键输入，如果不是释放对应技能直接返回
 * 如果是右键，则先判断当前是否有指向的敌人，如果有则直接释放技能，不过没有则进行移动相关处理
 */
void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag Tag)
{
	if (AuraAbilitySystemComponent && AuraAbilitySystemComponent->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}
	if (!Tag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		if (GetAuraAbilitySystemComponent())
		{
			GetAuraAbilitySystemComponent()->AbilityInputTagHeld(Tag);
		}
		return;
	}
	if (bTargeting || bShiftPressed)
	{
		if (GetAuraAbilitySystemComponent())
		{
			GetAuraAbilitySystemComponent()->AbilityInputTagHeld(Tag);
		}
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		APawn* ControlledPawn = GetPawn();
		if (CursorHitResult.bBlockingHit && ControlledPawn)
		{
			CachedDestination = CursorHitResult.ImpactPoint;
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning)return;
	if (APawn* ControlledPawn = GetPawn())
	{
		//获得样条线上距离玩家最近的点
		const FVector LocationOnSpline = SplineComponent->FindLocationClosestToWorldLocation(
			ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		//获得该点在样条线上的切线方向
		const FVector Direction = SplineComponent->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);
		const float DistanceToDestination = (CachedDestination - LocationOnSpline).Length();
		//如果距离目标点的距离小于可接受距离则停止自动寻路
		if (DistanceToDestination < AutoRunAcceptanceRadius)bAutoRunning = false;
	}
}

void AAuraPlayerController::UpdateMagicCircle()
{
	if (!IsValid(MagicCircle))return;
	MagicCircle->SetActorLocation(CursorHitResult.ImpactPoint);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent()
{
	if (!AuraAbilitySystemComponent)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (IsValid(MagicCircle) && DecalMaterial)
		{
			MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
			bShowMouseCursor = false;
		}
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
		bShowMouseCursor = true;
	}
}
