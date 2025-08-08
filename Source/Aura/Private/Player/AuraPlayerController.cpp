#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	//可被复制，用于网络复制
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();


	//断言：检测AuraContext是否已设置，如果没有设置，则强制停止执行
	check(AuraContext);
	//通过本地玩家的增强输入子系统，将InputMappingContext添加给本地玩家，并且可以设置优先级
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(AuraContext, 0);

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

	//InputComponent实际上指向的是一个UEnhancedInputComponent，CastChecked相当于Cast+check组合
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
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
	FHitResult CursorHitResult;
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHitResult);
	if (!CursorHitResult.bBlockingHit)return;

	LastActor=CurrentActor;
	AActor* HitActor=CursorHitResult.GetActor();
	if (HitActor&&HitActor->Implements<UEnemyInterface>())
	{
		CurrentActor=HitActor;
	}
	else
	{
		CurrentActor=nullptr;
	}
	/*
	 *当LastActor和CurrentActor都为空，或都不为空且相同时，什么都不做
	 *当LastActor为空,CurrentActor不为空时，CurrentActor调用HighlightActor
	 *当CurrentActor为空，LastActor不为空时，LastActor调用UnHighlightActor
	 *当都不为空时，且不相同，LastActor调用UnHighlightActor，CurrentActor调用HighlightActor
	 */
	if (!LastActor.IsValid())
	{
		if (CurrentActor.IsValid())
		{
			IEnemyInterface* CurrentPtr=Cast<IEnemyInterface>(CurrentActor.Get());
			CurrentPtr->HighlightActor();
		}
	}
	else
	{
		if (!CurrentActor.IsValid())
		{
			IEnemyInterface* LastPtr=Cast<IEnemyInterface>(LastActor.Get());
			LastPtr->UnHighlightActor();
		}
		else if (CurrentActor!=LastActor)
		{
			IEnemyInterface* CurrentPtr=Cast<IEnemyInterface>(CurrentActor.Get());
			IEnemyInterface* LastPtr=Cast<IEnemyInterface>(LastActor.Get());
			CurrentPtr->HighlightActor();
			LastPtr->UnHighlightActor();
		}
	}
}
