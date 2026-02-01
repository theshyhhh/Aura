#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; //希望投射物是可复制的，仅在服务器端处理移动，客户端只会看到复制版本

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SetRootComponent(SphereComponent);
	SphereComponent->SetCollisionObjectType(ECC_Projectile);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed = 550.f; //初始速度
	ProjectileMovementComponent->MaxSpeed = 550.f; //最大速度
	ProjectileMovementComponent->ProjectileGravityScale = 0.f; //重力
}

void AAuraProjectile::SetHomingTargetSceneComponent(USceneComponent* InHomingTargetSceneComponent)
{
	HomingTargetSceneComponent = InHomingTargetSceneComponent;
	ProjectileMovementComponent->HomingTargetComponent = InHomingTargetSceneComponent;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereBeginOverlap);

	//播放循环音效
	LoopingAudioComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	SetLifeSpan(5.f);
}

void AAuraProjectile::Destroyed()
{
	//服务器的销毁发送到客户端可能会先于客户端的Overlap，此时特效，声效还未播放，所以需要重写Destroy，播放一次特效、声效
	//如果播放过了，就直接销毁
	if (!bHit && !HasAuthority())
	{
		OnHit();
	}
	Super::Destroyed();
}

void AAuraProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//保证角色不会攻击到自己，且客户端还没有触发碰撞，保证生成音效特效只执行一次
	if (GetOwner() == OtherActor || bHit || !UAuraAbilitySystemLibrary::IsNotFriend(GetOwner(), OtherActor))return;
	OnHit();
	if (HasAuthority())
	{
		//GE的应用仅在服务器端执行，客户端属性会同步过去
		if (UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectParams.TargetGameplayAbilitySystem = TargetAsc;
			DamageEffectParams.DeathImpulse = DamageEffectParams.DeathImpulseMagnitude * GetActorForwardVector();
			//击退判定成功时设置击退的力
			if (FMath::RandRange(0.f, 100.f) < DamageEffectParams.KnockBackChance)
			{
				FRotator Rotation = GetActorRotation();
				Rotation.Pitch = 45.f;
				DamageEffectParams.KnockBackForce = DamageEffectParams.KnockBackMagnitude * Rotation.Vector();
			}
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		//如果是服务器端，直接销毁
		Destroy();
	}
}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (LoopingAudioComponent)
	{
		LoopingAudioComponent->Stop();
		LoopingAudioComponent->DestroyComponent();
	}
	bHit = true;
}
