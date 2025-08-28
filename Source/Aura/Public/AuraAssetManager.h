#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UAuraAssetManager& Get();

protected:
	//在引擎初始化早期被调用，项目级的初始化入口
	virtual void StartInitialLoading() override;
};
