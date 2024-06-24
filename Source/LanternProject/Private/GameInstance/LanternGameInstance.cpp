// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/LanternGameInstance.h"

#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "LanternProject/LanternProject.h"
#include "SaveGame/LanternSaveGame.h"
#include "Widget/ErrorWidget.h"

void ULanternGameInstance::Init()
{
	Super::Init();

	ErrorWidget = CreateWidget<UErrorWidget>(this, ErrorWidgetClass);
	check(ErrorWidget);

	if (USaveGame* LoadedSaveGame = UGameplayStatics::LoadGameFromSlot(SlotName, 0))
	{
		SaveGame = Cast<ULanternSaveGame>(LoadedSaveGame);
	}
	else
	{
		SaveGame = Cast<ULanternSaveGame>(UGameplayStatics::CreateSaveGameObject(ULanternSaveGame::StaticClass()));
	}

	check(SaveGame);

	LogFileIndex = SaveGame->LogFileIndex;

	InitFilePath();
	
	OpenLogFile();

	if (LoadConfig())
	{
		ApplyConfig();
		GetWorld()->GetTimerManager().SetTimer(MoveWindowTimerHandle, this, &ULanternGameInstance::MoveWindowFunction, 1.f, true);
	}
}

void ULanternGameInstance::Shutdown()
{
	Save();

	if (FileHandle)
	{
		delete FileHandle;
	}
	
	Super::Shutdown();
}

bool ULanternGameInstance::LoadFile(FString FileName, TArray<FString>& FileContent)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (FileManager.FileExists(*(FilePath + FileName)) == false) return false;

	return FFileHelper::LoadFileToStringArray(FileContent, *(FilePath + FileName));
}

bool ULanternGameInstance::LoadConfig()
{
	TArray<FString> FileContentArray;
	if (LoadFile(TEXT("Config.txt"), FileContentArray) == false) return false;

	for (FString& Iter : FileContentArray)
	{
		if (Iter.IsEmpty() || Iter[0] == '#') continue;
		FString LeftStr;
		FString RightStr;
		Iter.Split(FString("="), &LeftStr, &RightStr, ESearchCase::IgnoreCase);
		ConfigMap.Add(LeftStr, RightStr);
	}

	return true;
}

bool ULanternGameInstance::ApplyConfig()
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings == nullptr) return false;

	// Resolution
	FString* Width = ConfigMap.Find(FString("width"));
	if (Width == nullptr) return false;
	FString* Height = ConfigMap.Find(FString("height"));
	if (Height == nullptr) return false;

	// FullScreenMode
	FString* FullScreenMode = ConfigMap.Find(FString("fullscreenmode"));
	if (FullScreenMode == nullptr) return false;

	GameUserSettings->SetScreenResolution(FIntPoint(FCString::Atoi(**Width), FCString::Atoi(**Height)));
	GameUserSettings->SetFullscreenMode((EWindowMode::Type)FCString::Atoi(**FullScreenMode));

	GameUserSettings->ApplyResolutionSettings(false);

	return true;
}

void ULanternGameInstance::MoveWindowFunction()
{
	UEngine* Engine = GEngine;
	if (Engine != nullptr)
	{
		if (Engine->GameViewport != nullptr)
		{
			if (Engine->GameViewport->GetWindow())
			{
				// Position
				FString* XPos = ConfigMap.Find(FString("left"));
				FString* YPos = ConfigMap.Find(FString("top"));

				FVector2D WindowPosition = FVector2D(FCString::Atoi(**XPos), FCString::Atoi(**YPos));
				Engine->GameViewport->GetWindow()->MoveWindowTo(WindowPosition);

				GetWorld()->GetTimerManager().ClearTimer(MoveWindowTimerHandle);
			}
		}
	}
}

void ULanternGameInstance::OpenLogFile()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	FString LogFileName = FString::Printf(TEXT("LogFile%d.log"), LogFileIndex);

	// Check Directory
	PlatformFile.CreateDirectoryTree(*(FilePath + TEXT("Logs/")));
	
	// UE_LOG(LogTemp, Warning, TEXT("UBalloonGameInstance::LogToFile) Dir : %s"), *LogFilePath);

	FileHandle = PlatformFile.OpenWrite(*(FilePath + TEXT("Logs/") + LogFileName), true, true);
	if (FileHandle)
	{
		LogToFile(LOGTEXT(TEXT("Success to open log file")));
	}
	else
	{
		ShowToastMessage(TEXT("로그 열기 실패"));
	}
}

void ULanternGameInstance::LogToFile(const FString& InString)
{
	if (FileHandle)
	{
		FString NewLine = TEXT("[") + FDateTime::Now().ToString() + TEXT("] ") + InString + TEXT("\n");
		FileHandle->Write((const uint8*)TCHAR_TO_ANSI(*NewLine), NewLine.Len());
		
		if (FileHandle->Size() >= 100'000'000)
		{
			MoveToNextFile();
		}
	}
}

void ULanternGameInstance::MoveToNextFile()
{
	if (FileHandle)
	{
		delete FileHandle;

		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

		LogFileIndex = (LogFileIndex + 1) % 9;
		FString LogFileName = FString::Printf(TEXT("LogFile%d.log"), LogFileIndex);
		FString LogFilePath = FilePath + TEXT("Logs/");

		// Check directory
		PlatformFile.CreateDirectoryTree(*LogFilePath);

		LogFilePath += LogFileName;

		if (PlatformFile.FileExists(*LogFilePath))
		{
			PlatformFile.DeleteFile((*LogFilePath));
		}

		FileHandle = PlatformFile.OpenWrite(*LogFilePath, true, true);
		if (FileHandle)
		{
			UE_LOG(LogTemp, Warning, TEXT("LogFile is opened"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LogFile is not opened"));
		}
	}
}

void ULanternGameInstance::Save()
{
	if (SaveGame)
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			PC->Pause();
		}

		SaveGame->LogFileIndex = LogFileIndex;

		UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, 0);

		LogToFile(LOGTEXT(TEXT("Data is saved")));
	}
	else
	{
		LogToFile(LOGTEXT(TEXT("SaveData is invalid")));
	}
}

void ULanternGameInstance::SaveLanternArray(const TArray<ALanternActor*>& LanternArray)
{
	if (SaveGame)
	{
		SaveGame->SaveLanternArray(LanternArray);
		UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, 0);
	}
}

void ULanternGameInstance::LoadLanternArray(ALanternManager* LanternManager)
{
	if (SaveGame)
	{
		SaveGame->LoadLanternArray(LanternManager);
		UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, 0);
	}
}

void ULanternGameInstance::SaveMqttMessageArray(const TArray<FString>& MqttMessageArray)
{
	if (SaveGame)
	{
		SaveGame->SaveMqttMessageArray(MqttMessageArray);
	}
}

void ULanternGameInstance::LoadMqttMessageArray(AMqttManager* MqttManager)
{
	if (SaveGame)
	{
		SaveGame->LoadMqttMessageArray(MqttManager);
	}
}

ULanternSaveGame* ULanternGameInstance::GetSaveGame() const
{
	return SaveGame;
}

const TMap<FString, FString>& ULanternGameInstance::GetConfigMap() const
{
	return ConfigMap;
}

const FString& ULanternGameInstance::GetFilePath() const
{
	return FilePath;
}

void ULanternGameInstance::ShowAlertMessage(const FString& Message)
{
	if (ErrorWidget->IsInViewport() == false)
	{
		ErrorWidget->AddToViewport();
	}

	GetWorld()->GetAuthGameMode();

	ErrorWidget->ShowAlertMessage(Message);
}

void ULanternGameInstance::ShowToastMessage(const FString& Message)
{
	if (ErrorWidget->IsInViewport() == false)
	{
		ErrorWidget->AddToViewport();
	}

	ErrorWidget->ShowToastMessage(Message);
}

void ULanternGameInstance::InitFilePath()
{
	FilePath = FPlatformProcess::UserDir();
	FilePath += TEXT("MRAG/Interactive/LanternProject");
}
