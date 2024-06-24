// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LanternSaveGame.generated.h"

/**
 * 
 */

class ALanternActor;

USTRUCT()
struct FLanternData
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FColor> TextureColors;
	UPROPERTY()
	FIntPoint TextureSize;
	UPROPERTY()
	int32 TypeIndex;
	UPROPERTY()
	int32 VariationIndex;
	UPROPERTY()
	FTransform Transform;
	UPROPERTY()
	FVector BackDirection;

	UPROPERTY()
	TArray<uint32> OverlappedLanternIndexArray;

	void Print()
	{
		UE_LOG(LogTemp, Warning, TEXT("TextureSize : %s, TypeIndex : %d, VariationIndex : %d, Transform : %s, BackDirection : %s"), *TextureSize.ToString(), TypeIndex, VariationIndex, *Transform.ToString(), *BackDirection.ToString());
	}
};

UCLASS()
class LANTERNPROJECT_API ULanternSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	void AddCount();

	int32 GetCount() const;

	void SaveLanternArray(const TArray<ALanternActor*>& LanternArray);

	void LoadLanternArray(class ALanternManager* LanternManager);

	void SaveMqttMessageArray(const TArray<FString>& NewMqttMessageArray);

	void LoadMqttMessageArray(class AMqttManager* MqttManager);

	// Property
	UPROPERTY()
	int32 LogFileIndex = 0;

private:
	UPROPERTY(Meta = (AllowPrivateAccess))
	TArray<FString> MqttMessageArray;
	
	UPROPERTY(Meta = (AllowPrivateAccess))
	TArray<FLanternData> LanternDataArray;
	UPROPERTY(Meta = (AllowPrivateAccess))
	int32 TestCount = 0;
};
