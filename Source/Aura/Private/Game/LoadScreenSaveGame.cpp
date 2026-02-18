// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LoadScreenSaveGame.h"

FMapSaveInfo ULoadScreenSaveGame::GetMapSaveInfoByName(const FString& InMapName)
{
	for (const FMapSaveInfo& SaveInfo : SavedMaps)
	{
		if (SaveInfo.MapAssetName == InMapName)return SaveInfo;
	}
	return FMapSaveInfo();
}

bool ULoadScreenSaveGame::HasMap(const FString& InMapName)
{
	for (const FMapSaveInfo& SaveInfo : SavedMaps)
	{
		if (SaveInfo.MapAssetName == InMapName)return true;
	}
	return false;
}
