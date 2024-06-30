// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SoundManager.h"

#include "Components/AudioComponent.h"
#include "GameInstance/LanternGameInstance.h"
#include "LanternProject/LanternProject.h"

// Sets default values
ASoundManager::ASoundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AmbientSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AmbientSoundComponent"));
	SetRootComponent(AmbientSoundComponent);
}

// Called when the game starts or when spawned
void ASoundManager::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

	LoadSoundFile();
}

// Called every frame
void ASoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASoundManager::LoadSoundFile()
{
	ULanternGameInstance* GameInstance = GetGameInstance<ULanternGameInstance>();
	if (GameInstance->IsValidLowLevelFast() == false) return;

	const FString* pFileName = GameInstance->GetConfigMap().Find(TEXT("AmbientSoundFileName"));
	if (pFileName == nullptr)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Config file has no AmbientSoundFileName Field")));
		return;
	}
	
	FString SoundPath = GameInstance->GetFilePath() + TEXT("Sounds/") + *pFileName;

	TArray<uint8> RawFileData;
	if (FFileHelper::LoadFileToArray(RawFileData, *SoundPath) == false)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Invalid file name")));
		return;
	}

	SoundWave = NewObject<USoundWave>(USoundWave::StaticClass());

	if (SoundWave->IsValidLowLevelFast() == false)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Failed to create soundwave")));
		return;
	}
	
	FWaveModInfo WaveInfo;
	if (WaveInfo.ReadWaveInfo(RawFileData.GetData(), RawFileData.Num()))
	{

		// From FSoundWavePCMWriter::ApplyBufferToSoundWave() UE4.24
		SoundWave->SetSampleRate(*WaveInfo.pSamplesPerSec);
		SoundWave->NumChannels = *WaveInfo.pChannels;

		const int32 BytesDataPerSecond = *WaveInfo.pChannels * (*WaveInfo.pBitsPerSample / 8.f) * *WaveInfo.pSamplesPerSec;
		if (BytesDataPerSecond)
		{
			SoundWave->Duration = WaveInfo.SampleDataSize / BytesDataPerSecond;
		}
		SoundWave->RawPCMDataSize = WaveInfo.SampleDataSize;

		SoundWave->RawPCMData = static_cast<uint8*>(FMemory::Malloc(WaveInfo.SampleDataSize));
		FMemory::Memcpy(SoundWave->RawPCMData, WaveInfo.SampleDataStart, WaveInfo.SampleDataSize);
		SoundWave->bLooping = true;
		
		AmbientSoundComponent->SetSound(SoundWave);
		AmbientSoundComponent->Play();
	}
	else
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Failed to read raw file data")));
		return;
	}
}

