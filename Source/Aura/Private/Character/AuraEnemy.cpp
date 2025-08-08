#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	
	GetMesh()->SetRenderCustomDepth(false);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(false);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::HighlightActor()
{
	SetRenderCustomDepth(true);
}

void AAuraEnemy::UnHighlightActor()
{
	SetRenderCustomDepth(false);
}

void AAuraEnemy::SetRenderCustomDepth(const bool bValue)
{
	GetMesh()->SetRenderCustomDepth(bValue);
	Weapon->SetRenderCustomDepth(bValue);
}
