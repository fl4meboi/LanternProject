// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/LanternSaveGame.h"

#include "LanternActor.h"
#include "Actor/LanternManager.h"
#include "Actor/MqttManager.h"

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
	LanternManager->LoadLantern(LanternDataArray);
	LanternDataArray.Empty();
}

void ULanternSaveGame::SaveMqttMessageArray(const TArray<FString>& NewMqttMessageArray)
{
	MqttMessageArray = NewMqttMessageArray;
}

void ULanternSaveGame::LoadMqttMessageArray(AMqttManager* MqttManager)
{
	MqttManager->LoadMqttMessageArray(MqttMessageArray);
	MqttMessageArray.Empty();
}
