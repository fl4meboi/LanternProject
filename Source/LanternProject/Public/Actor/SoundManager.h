// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundManager.generated.h"

UCLASS()
class LANTERNPROJECT_API ASoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void LoadSoundFile();

private:
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess))
	USoundBase* AmbientSound;
	UAudioComponent* AmbientSoundComponent;

	UPROPERTY()
	USoundWave* SoundWave;
};
