// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/LanternSaveGame.h"

#include "LanternActor.h"

void ULanternSaveGame::AddCount()
{
	TestCount++;
}

int32 ULanternSaveGame::GetCount() const
{
	return TestCount;
}

void ULanternSaveGame::SaveLanternArray(const TArray<ALanternActor*>& LanternArray)
{
	for (auto Lantern : LanternArray)
	{
		Lantern->SetLanternArrayIndex(LanternArray);
	}
	for (auto Lantern : LanternArray)
	{
		FLanternData LanternData;
		Lantern->SaveData(LanternData);

		LanternDataArray.Add(LanternData);
	}
}

void ULanternSaveGame::LoadLanternArray(ALanternManager* LanternManager)
{
}

void ULanternSaveGame::SaveMqttMessageArray(const TArray<FString>& NewMqttMessageArray)
{
}

void ULanternSaveGame::LoadMqttMessageArray(AMqttManager* MqttManager)
{
}
