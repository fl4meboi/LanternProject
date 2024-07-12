// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LanternFloatActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ALanternFloatActor::ALanternFloatActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
	// LanternMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("LanternMesh");
	// LanternMeshComponent->SetupAttachment(RootComponent);
	// LanternMeshComponent->SetCollisionProfileName(FName("OverlapAllDynamic"));
}	

// Called when the game starts or when spawned
void ALanternFloatActor::BeginPlay()
{
	Super::BeginPlay();

	CreateMeshComponents();
	RandomSpawn();

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

// Called every frame
void ALanternFloatActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	for (UStaticMeshComponent* MeshComponent : MeshComponents)
	{
		if (MeshComponent)
		{
			FVector NewLocation = MeshComponent->GetComponentLocation();
			NewLocation.Z += UpSpeed * DeltaTime;
			MeshComponent->SetWorldLocation(NewLocation);
		}
	}
}

void ALanternFloatActor::CreateMeshComponents()
{
	for (int32 i = 0; i < NumMeshes; ++i)
	{
		UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("MeshComponent%d"), i));
		MeshComponent->SetupAttachment(RootComponent);
		MeshComponents.Add(MeshComponent);
	}
}

void ALanternFloatActor::RandomSpawn()
{
	for (UStaticMeshComponent* MeshComponent : MeshComponents)
	{
		if (MeshComponent)
		{
			FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox((MinSpawnLocation + MaxSpawnLocation) / 2, (MaxSpawnLocation - MinSpawnLocation) / 2);
			MeshComponent->SetWorldLocation(SpawnLocation);
		}
	}
}


