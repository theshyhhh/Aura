#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"


AAuraCharacter::AAuraCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	//将角色固定在一个平面上
	GetCharacterMovement()->bConstrainToPlane = true;
	//如果bConstrainToPlane为真，则bSnapToPlaneAtStart为真会使角色开始时就固定在一个平面上
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	//禁用控制器旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//服务端初始化
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//客户端初始化
	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
	AttributeSet = AuraPlayerState->GetAttributeSet();
	//初始化UI
	//只有当前客户端的玩家才有控制器，其它客户端的玩家在当前客户端没有控制器，所以其它人物就不会在本客户端初始化HUD
	if (APlayerController* PlayerController = GetController<APlayerController>())
	{
		AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD());
		check(AuraHUD);
		AuraHUD->InitOverlayWidget(PlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
	}
}
