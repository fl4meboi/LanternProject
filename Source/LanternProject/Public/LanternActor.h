// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "LanternActor.generated.h"

UCLASS()
class LANTERNPROJECT_API ALanternActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALanternActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector CameraLocation;
	FVector BaseLocation;
	FVector TargetLocation;

	float WidthLimit;
	float HeightLimit;
	float Distance;

	UPROPERTY(BlueprintReadOnly)
	FVector BackDirection;

	UPROPERTY()
	FRotator BaseRotation;
	
	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* CapsuleComponent;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* LanternMeshComponent;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float UpSpeed = 3.f;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float SideSpeed = 1.f;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float BackSpeed = 2.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
