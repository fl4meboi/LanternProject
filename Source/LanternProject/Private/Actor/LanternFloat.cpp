// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LanternFloat.h"

// Sets default values
ALanternFloat::ALanternFloat()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
}

// Called when the game starts or when spawned
void ALanternFloat::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALanternFloat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALanternFloat::SetMesh(UStaticMesh* NewMesh)
{
	if (NewMesh)
	{
		MeshComponent->SetStaticMesh(NewMesh);
	}
}

