// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LanternFloatManager.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ALanternFloatManager::ALanternFloatManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALanternFloatManager::BeginPlay()
{
	Super::BeginPlay();
	

	SpawnedLanternFloat(300);
}

// Called every frame
void ALanternFloatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALanternFloatManager::SpawnedLanternFloat(int32 NumObjects)
{
	UWorld* World = GetWorld();
	if (World && SpawnedLanternFloats.Num() > 0)
	{
		for (int32 i = 0; i < NumObjects; i++)
		{
			FVector SpawnLocation = GetRandomSpawnLocation();
			FActorSpawnParameters SpawnParams;

			TSubclassOf<AActor> RandomLanternFLoat = GetRandomLanternFloat();
			if (RandomLanternFLoat)
			{
				World->SpawnActor<AActor>(RandomLanternFLoat, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			}
		}
	}
}

FVector ALanternFloatManager::GetRandomSpawnLocation()
{
	return UKismetMathLibrary::RandomPointInBoundingBox((SpawnAreaMin + SpawnAreaMax) / 2, (SpawnAreaMax - SpawnAreaMin) / 2);
}

TSubclassOf<AActor> ALanternFloatManager::GetRandomLanternFloat()
{
	if (SpawnedLanternFloats.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, SpawnedLanternFloats.Num() - 1);
		return SpawnedLanternFloats[RandomIndex];
	}

	return nullptr;
}

