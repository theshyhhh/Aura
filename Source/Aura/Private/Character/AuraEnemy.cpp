#include "Character/AuraEnemy.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AI/AuraAIController.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetMesh()->SetRenderCustomDepth(false);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(false);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>("HealthBarComponent");
	HealthBarComponent->SetupAttachment(GetRootComponent());

	BaseWalkSpeed = 250.f;
}

void AAuraEnemy::HighlightActor_Implementation()
{
	SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighlightActor_Implementation()
{
	SetRenderCustomDepth(false);
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget.Get();
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallBackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if (HasAuthority() && AuraAIController && AuraAIController->GetBlackboardComponent())
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
}

void AAuraEnemy::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);
	if (AuraAIController)AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), true);
	Super::Die(DeathImpulse);
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!HasAuthority())return;
	AuraAIController = Cast<AAuraAIController>(NewController);
	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->GetBlackboardAsset());
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	if (HasAuthority())UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	if (UAuraUserWidget* HealthBarWidget = Cast<UAuraUserWidget>(HealthBarComponent->GetUserWidgetObject()))
	{
		HealthBarWidget->SetWidgetController(this); //自身作为UI的控制器
	}

	if (const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue); //属性变化时，广播给UI
			});
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});
		AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &AAuraEnemy::HitReactTagChanged);
		AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &AAuraEnemy::OnStunTagChanged);
		//初始化广播
		OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	}
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	//保证只在服务器上进行初始化行为，属性会同步到客户端，主要因为初始化要使用的角色职业信息在GameMode里，而GameMode只在服务器上存在
	if (HasAuthority())InitializeDefaultAttributes();
	OnASCRegisteredDelegate.Broadcast(AbilitySystemComponent);
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AAuraEnemy::OnStunTagChanged(const FGameplayTag CallBackTag, int32 NewCount)
{
	Super::OnStunTagChanged(CallBackTag, NewCount);
	if (HasAuthority() && AuraAIController && AuraAIController->GetBlackboardComponent())
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsStunned"), bIsStunned);
}

void AAuraEnemy::SetRenderCustomDepth(const bool bValue) const
{
	GetMesh()->SetRenderCustomDepth(bValue);
	Weapon->SetRenderCustomDepth(bValue);
}
