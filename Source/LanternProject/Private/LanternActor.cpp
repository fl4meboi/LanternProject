// Fill out your copyright notice in the Description page of Project Settings.


#include "LanternActor.h"

// Sets default values
ALanternActor::ALanternActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
	LanternMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("LanternMesh");
	LanternMeshComponent->SetupAttachment(RootComponent);
	LanternMeshComponent->SetCollisionProfileName(FName("OverlapAllDynamic"));
}

// Called when the game starts or when spawned
void ALanternActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALanternActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorRotation(FRotator(0, (CameraLocation - GetActorLocation()).Rotation().Yaw, 0));

	
	// FVector ActorLocation = GetActorLocation();
	//
	// // Back
	// float CurrentDistance = FMath::Abs(ActorLocation.X - BaseLocation.X);
	//
	// // 현재 거리비
	// float DistanceAlpha = CurrentDistance/Distance;
	// // Center Lerp
	// FVector CurrentCenter = FMath::Lerp(BaseLocation, TargetLocation, DistanceAlpha);
	// // 현재 최대 좌우 거리
	// float CurrentWidthLimit = FMath::Lerp(0, WidthLimit, DistanceAlpha);
	// // 현재 최대 상하 거리
	// float CurrentHeightLimit = FMath::Lerp(0, HeightLimit, DistanceAlpha);
	//
	// if (CurrentDistance <= Distance)
	// {
	// 	AddActorWorldOffset(FVector(BackDirection.X, 0, 0) * BackSpeed * DeltaTime);
	// }
	// if (FMath::Abs(ActorLocation.Y - BaseLocation.Y) < CurrentWidthLimit)
	// {
	// 	AddActorWorldOffset(FVector(0, BackDirection.Y, 0) * SideSpeed * DeltaTime);
	// }
	//
	// FVector Location = GetActorLocation();
	//
	// Location.Z = FMath::Lerp(BaseLocation.Z, (BaseLocation + (BackDirection * Distance)).Z, DistanceAlpha);
	// SetActorLocation(Location);
	//
	// AddActorWorldOffset(FVector(0, 0, BackDirection.Z) * UpSpeed * DeltaTime);
	
	FVector UpwardMovement = FVector(0.f, 0.f, UpSpeed * DeltaTime);

	AddActorWorldOffset(UpwardMovement, true);
}

