// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LanternGameModeBase.h"

#include "EngineUtils.h"
#include "Actor/LanternManager.h"
#include "GameFramework/PlayerStart.h"
#include "GameInstance/LanternGameInstance.h"
#include "LanternProject/LanternProject.h"

void ALanternGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GameInstance = GetGameInstance<ULanternGameInstance>();
	check(GameInstance->IsValidLowLevelFast());

	TActorIterator<APlayerStart> It(GetWorld());
	if (*It != nullptr)
	{
		for (;It;++It)
		{
			int32 PlayerStartIndex = FCString::Atoi(*(*It)->PlayerStartTag.ToString());
			if (PlayerStartIndex >= PawnTypeArray.Num())
			{
				GameInstance->LogToFile(LOGTEXT(TEXT("%s is bigger than pawntype array length"), *(*It)->GetActorNameOrLabel()));
				continue;
			}

			PawnTypeArray[PlayerStartIndex].PlayerStart = (*It);
		}
	}

	if (PawnTypeArray.Num() <= 0) return;

	const FString* pPawnType = GameInstance->GetConfigMap().Find(TEXT("PawnType"));
	if (pPawnType == nullptr)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Config has no PawnType field")));
		return;
	}

	PawnTypeIndex = FCString::Atoi(**pPawnType);
	if (PawnTypeIndex >= PawnTypeArray.Num())
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("PawnTypeIndex is invalid")));
		return;
	}

	DefaultPawnClass = PawnTypeArray[PawnTypeIndex].PawnClass;

	GameInstance->LogToFile(LOGTEXT(TEXT("PawnClass Name : %s"), *DefaultPawnClass->GetName()));

	TActorIterator<ALanternManager> LanternManagerIt(GetWorld());
	if (*LanternManagerIt == nullptr)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Level has no LanternManager")));
		return;
	}

	(*LanternManagerIt)->SetLimitLocation(PawnTypeArray[PawnTypeIndex].UpLimitLocation, PawnTypeArray[PawnTypeIndex].RightLimitLocation, PawnTypeArray[PawnTypeIndex].TargetLocation);
	(*LanternManagerIt)->SetSpawnLocation(PawnTypeArray[PawnTypeIndex].SpawnLocation);
}

void ALanternGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

AActor* ALanternGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	if (PawnTypeArray.Num() > 0 && PawnTypeIndex != -1)
	{
		if (PawnTypeArray[PawnTypeIndex].PlayerStart->IsValidLowLevelFast())
		{
			GameInstance->LogToFile(LOGTEXT(TEXT("Player Start Location : %s"), *PawnTypeArray[PawnTypeIndex].PlayerStart->GetActorLocation().ToString()));
		}
		else
		{
			GameInstance->LogToFile(LOGTEXT(TEXT("Player Start Location is invalid")));
		}

		return PawnTypeArray[PawnTypeIndex].PlayerStart;
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}
