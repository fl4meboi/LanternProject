// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LanternFloat.h"
#include "LanternFloatManager.generated.h"

UCLASS()
class LANTERNPROJECT_API ALanternFloatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALanternFloatManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Lantern")
	TArray<TSubclassOf<AActor>> SpawnedLanternFloats;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess, MakeEditWidget, Category = "Lantern"))
	FVector SpawnAreaMin;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess, MakeEditWidget, Category = "Lantern"))
	FVector SpawnAreaMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lantern")
	TSubclassOf<ALanternFloat> SpawnedLanternFloatClass;

	UFUNCTION(BlueprintCallable, Category = "Lantern")
	void SpawnedLanternFloat(int32 NumObjects);

	UFUNCTION(BlueprintCallable, Category = "Lantern")
	FVector GetRandomSpawnLocation();

private: 
	TSubclassOf<AActor> GetRandomLanternFloat();
};
