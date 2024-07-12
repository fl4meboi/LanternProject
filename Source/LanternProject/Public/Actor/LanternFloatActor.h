// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LanternFloatActor.generated.h"

class ALanternManager;

struct FLanternData;
UCLASS()
class LANTERNPROJECT_API ALanternFloatActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALanternFloatActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RandomSpawn();
	void CreateMeshComponents();

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UStaticMeshComponent* LanternMeshComponent;
	
	UPROPERTY(EditAnywhere, Category = "Lantern")
	float UpSpeed = 3.f;

	UPROPERTY(EditAnywhere, Category = "Lantern")
	int32 NumMeshes = 10;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess, MakeEditWidget))
	FVector MinSpawnLocation;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess, MakeEditWidget))
	FVector MaxSpawnLocation;

private:
	UPROPERTY(VisibleAnywhere)
	TArray<UStaticMeshComponent*> MeshComponents;

};
