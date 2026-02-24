#include "Character/AuraCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"


AAuraCharacter::AAuraCharacter()
{
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LevelUpNiagaraComponent"));
	LevelUpNiagaraComponent->SetupAttachment(RootComponent);
	LevelUpNiagaraComponent->bAutoActivate = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

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

int32 AAuraCharacter::GetCharacterLevel_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);

	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda([this]()
	{
		if (AAuraGameModeBase* AuraGM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGM->PlayerDied(this);
		}
	});
	GetWorldTimerManager().SetTimer(DeathTimerHandle, DeathTimerDelegate, DeathTime, false);
	Camera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();
	//初始化玩家角色属性信息
	LoadProgress();
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

void AAuraCharacter::AddXP_Implementation(const int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddXP(InXP);
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticle();
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerXP();
}

int32 AAuraCharacter::FindLevelByXP_Implementation(const int32 XP)
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->FindLevelByXP(XP);
}

int32 AAuraCharacter::GetAttributePointReward_Implementation(const int32 Level) const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->LevelUpInfo[Level - 2].AttributePointsReward;
}

int32 AAuraCharacter::GetSpellPointReward_Implementation(const int32 Level) const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->LevelUpInfo[Level - 2].SpellPointReward;
}

void AAuraCharacter::AddPlayerLevel_Implementation(const int32 InLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddLevel(InLevel);
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		AuraASC->UpdateAbilityStatus(AuraPlayerState->GetPlayerLevel());
	}
}

void AAuraCharacter::AddAttributePoint_Implementation(const int32 InAttributePoint)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddAttributePoint(InAttributePoint);
}

void AAuraCharacter::AddSpellPoint_Implementation(const int32 InSpellPoint)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddSpellPoint(InSpellPoint);
}

int32 AAuraCharacter::GetPlayerAttributePoint_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoint();
}

int32 AAuraCharacter::GetPlayerSpellPoint_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoint();
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AAuraPlayerController* AuraPlayerController = GetController<AAuraPlayerController>())
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* AuraPlayerController = GetController<AAuraPlayerController>())
	{
		AuraPlayerController->HideMagicCircle();
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	if (!HasAuthority())return;
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	check(AuraGameMode);
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	check(AuraGameInstance);
	AuraGameInstance->PlayerStartTag = CheckpointTag;
	ULoadScreenSaveGame* SaveGame = AuraGameMode->GetSaveSlotData(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex);
	SaveGame->PlayerStartTag = CheckpointTag;
	AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
	check(AuraPlayerState);
	SaveGame->Level = AuraPlayerState->GetPlayerLevel();
	SaveGame->XP = AuraPlayerState->GetPlayerXP();
	SaveGame->AttributePoint = AuraPlayerState->GetAttributePoint();
	SaveGame->SpellPoint = AuraPlayerState->GetSpellPoint();
	SaveGame->bFirstTimeLoadIn = false;
	SaveGame->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
	SaveGame->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
	SaveGame->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
	SaveGame->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());
	FForEachAbility SaveAbilityDelegate;
	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	SaveGame->SavedAbilities.Empty();
	SaveAbilityDelegate.BindLambda([AuraASC,SaveGame,AuraGameMode](const FGameplayAbilitySpec& Spec)-> void
	{
		FAbilitySaveInfo AbilitySaveInfo;
		AbilitySaveInfo.AbilityClass = Spec.Ability->GetClass();
		AbilitySaveInfo.AbilityTag = UAuraAbilitySystemComponent::GetAbilityTagFromAbilitySpec(Spec);
		FAuraAbilityInfo AbilityInfo = AuraGameMode->AbilityInfo->FindAbilityByTag(AbilitySaveInfo.AbilityTag);
		AbilitySaveInfo.AbilityType = AbilityInfo.AbilityType;
		AbilitySaveInfo.AbilityLevel = Spec.Level;
		AbilitySaveInfo.AbilityInputTag = AuraASC->GetInputTagByAbilityTag(AbilitySaveInfo.AbilityTag);
		AbilitySaveInfo.AbilityStatus = AuraASC->GetStatusTagByAbilityTag(AbilitySaveInfo.AbilityTag);

		SaveGame->SavedAbilities.AddUnique(AbilitySaveInfo);
	});
	AuraASC->ForEachAbility(SaveAbilityDelegate);
	UGameplayStatics::SaveGameToSlot(SaveGame, AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex);
}


void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	OnASCRegisteredDelegate.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this, &AAuraCharacter::OnStunTagChanged);
	//初始化UI
	//只有当前客户端的玩家才有控制器，其它客户端的玩家在当前客户端没有控制器，所以其它人物就不会在本客户端初始化HUD
	if (APlayerController* PlayerController = GetController<APlayerController>())
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD()))
		{
			AuraHUD->InitOverlayWidget(PlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}

void AAuraCharacter::LoadProgress()
{
	if (!HasAuthority())return;
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	check(AuraGameMode);
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	check(AuraGameInstance);
	ULoadScreenSaveGame* SaveGame = AuraGameMode->GetSaveSlotData(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex);
	check(SaveGame);
	AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
	check(AuraPlayerState);
	AuraPlayerState->SetLevel(SaveGame->Level);
	AuraPlayerState->SetXP(SaveGame->XP);
	AuraPlayerState->SetAttributePoint(SaveGame->AttributePoint);
	AuraPlayerState->SetSpellPoint(SaveGame->SpellPoint);
	if (SaveGame->bFirstTimeLoadIn)
	{
		InitializeDefaultAttributes();
		AddCharacterAbilities();
	}
	else
	{
		InitPrimaryAttributeFromSaveGame(SaveGame);
		ApplyEffectToSelf(DefaultSecondaryAttributes, Execute_GetCharacterLevel(this));
		ApplyEffectToSelf(InitVitalAttributes, Execute_GetCharacterLevel(this));
		if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
		{
			AuraASC->AddCharacterAbilitiesFromSaveData(SaveGame);
		}
	}
	AuraGameMode->LoadWorldState(GetWorld());
}

void AAuraCharacter::InitPrimaryAttributeFromSaveGame(const ULoadScreenSaveGame* SaveGame)
{
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(PrimaryAttributeEffect_SetByCaller, 1.f, ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Attributes_Primary_Strength,
	                                                              SaveGame->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Attributes_Primary_Intelligence,
	                                                              SaveGame->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Attributes_Primary_Resilience,
	                                                              SaveGame->Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Attributes_Primary_Vigor, SaveGame->Vigor);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

void AAuraCharacter::MulticastLevelUpParticle_Implementation() const
{
	if (!IsValid(LevelUpNiagaraComponent))return;
	const FVector CameraLocation = Camera->GetComponentLocation();
	const FVector NiagaraLocation = LevelUpNiagaraComponent->GetComponentLocation();
	const FRotator NiagaraToCameraRotation = (CameraLocation - NiagaraLocation).Rotation();
	LevelUpNiagaraComponent->SetWorldRotation(NiagaraToCameraRotation);
	LevelUpNiagaraComponent->Activate(true);
}
