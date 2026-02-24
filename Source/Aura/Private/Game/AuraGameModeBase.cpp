#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannel.h"
#include "Game/AuraGameInstance.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlotViewModel.h"

void AAuraGameModeBase::SaveSlotData(const UMVVM_LoadSlotViewModel* LoadSlotViewModel, int32 SlotIndex) const
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlotViewModel->GetLoadSlotName(), SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlotViewModel->GetLoadSlotName(), SlotIndex);
	}
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSavaGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlotViewModel->GetPlayerName();
	LoadScreenSaveGame->SaveSlotStatus = LoadSlotViewModel->SaveSlotStatus;
	LoadScreenSaveGame->MapName = LoadSlotViewModel->GetMapName();
	LoadScreenSaveGame->PlayerStartTag = LoadSlotViewModel->PlayerStartTag;
	LoadScreenSaveGame->MapAssetName = LoadSlotViewModel->MapAssetName;
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlotViewModel->GetLoadSlotName(), SlotIndex);
}

void AAuraGameModeBase::SaveWorldState(UWorld* World, const FString& InMapAssetName) const
{
	FString MapName = World->GetMapName();
	//移除前缀
	MapName.RemoveFromStart(World->StreamingLevelsPrefix);
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGameInstance);
	if (ULoadScreenSaveGame* SaveGame = GetSaveSlotData(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		if (InMapAssetName != FString(""))
		{
			SaveGame->MapAssetName = InMapAssetName;
			SaveGame->MapName = GetMapNameFromMapAssetName(InMapAssetName);
		}
		if (!SaveGame->HasMap(MapName))
		{
			FMapSaveInfo MapSaveInfo;
			MapSaveInfo.MapAssetName = MapName;
			SaveGame->SavedMaps.Add(MapSaveInfo);
		}
		FMapSaveInfo SavedMap = SaveGame->GetMapSaveInfoByName(MapName);
		SavedMap.SavedActors.Empty();
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>())continue;
			FActorSaveInfo SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetActorTransform();
			FMemoryWriter Writer(SavedActor.Bytes);
			//用于序列化UObject和FName
			FObjectAndNameAsStringProxyArchive Archive(Writer, true);
			//只有标了 UPROPERTY(SaveGame)（或蓝图变量勾了 SaveGame）的字段会被写进你的 ByteData
			Archive.GetArchiveState().ArIsSaveGame = true;
			Actor->Serialize(Archive);
			SavedMap.SavedActors.AddUnique(SavedActor);
		}
		//替换原存档
		for (FMapSaveInfo& Info : SaveGame->SavedMaps)
		{
			if (Info.MapAssetName == SavedMap.MapAssetName)
			{
				Info = SavedMap;
			}
		}
		UGameplayStatics::SaveGameToSlot(SaveGame, AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex);
	}
}

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	FString MapName = World->GetMapName();
	//移除前缀
	MapName.RemoveFromStart(World->StreamingLevelsPrefix);
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGameInstance);
	if (UGameplayStatics::DoesSaveGameExist(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		ULoadScreenSaveGame* SaveGame = Cast<ULoadScreenSaveGame>(
			UGameplayStatics::LoadGameFromSlot(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex));
		if (SaveGame == nullptr)
		{
			UE_LOG(LogAura, Warning, TEXT("LoadWorldState: Can't find save game"));
			return;
		}
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor->Implements<USaveInterface>())continue;
			UE_LOG(LogTemp, Warning, TEXT("世界状态加载"))
			for (FActorSaveInfo Info : SaveGame->GetMapSaveInfoByName(MapName).SavedActors)
			{
				if (Info.ActorName == Actor->GetFName())
				{
					if (ISaveInterface::Execute_ShouldSetTransform(Actor))
					{
						Actor->SetActorTransform(Info.Transform);
					}
					FMemoryReader Reader(Info.Bytes);
					FObjectAndNameAsStringProxyArchive Archive(Reader, true);
					Archive.GetArchiveState().ArIsSaveGame = true;
					Actor->Serialize(Reader);
					ISaveInterface::Execute_LoadActor(Actor);
					break;
				}
			}
		}
	}
}

void AAuraGameModeBase::DeleteSlotData(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSavaGameClass);
	}
	return Cast<ULoadScreenSaveGame>(SaveGameObject);
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlotViewModel* LoadSlotVM)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, Maps[LoadSlotVM->GetMapName()]);
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	if (PlayerStarts.Num() > 0)
	{
		for (AActor* Actor : PlayerStarts)
		{
			if (APlayerStart* PS = Cast<APlayerStart>(Actor))
			{
				if (PS->PlayerStartTag == AuraGameInstance->PlayerStartTag)return PS;
			}
		}
		return PlayerStarts[0];
	}
	return nullptr;
}

FString AAuraGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (const auto& Map : Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetFName() == MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString("");
}

void AAuraGameModeBase::PlayerDied(ACharacter* DeadCharacter)
{
	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	ULoadScreenSaveGame* SaveGame = GetSaveSlotData(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex);
	if (!IsValid(SaveGame))return;
	UGameplayStatics::OpenLevel(DeadCharacter, FName(SaveGame->MapAssetName));
}
