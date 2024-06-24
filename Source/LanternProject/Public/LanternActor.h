// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "LanternActor.generated.h"

class ALanternManager;
class UCapsuleComponent;
class UUserWidget;

struct FLanternData;
UCLASS()
class LANTERNPROJECT_API ALanternActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALanternActor();

	UFUNCTION(BlueprintCallable)
	UTexture2D* TextureFromWidget(UUserWidget* const Widget, const FVector2D& DrawSize);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetLanternSpeed(float NewBackSpeed = -1.f, float NewUpSpeed = -1.f, float NewSideSpeed = -1.f);

	void Init(UTexture2D* LanternImage, const FVector& NewBackDirection, int32 NewTypeIndex, int32 NewVariationIndex);
	void SetData(const FVector& NewCameraLocation, const FVector& NewBaseLocation, const FVector NewTargetLocation, float NewWidthLimit, float NewHeightLimit, float NewDistance);

	UFUNCTION()
	void OnLanternMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnLanternMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FVector GetSeparation();

	// Save / Load NeighborLantern
	void  AddNeighborLantern(ALanternActor* NewNeighborLantern);

	void SetLanternArrayIndex(const TArray<ALanternActor*>& LanternArray);
	int32 GetLanternArrayIndex() const;

	// Save
	void SaveData(FLanternData& LanternData);

protected:
	// 공통 변수

	// Camera(Pawn) 위치
	FVector CameraLocation;
	// LanternManager 위치
	FVector BaseLocation;
	// Lantern 도착지 중앙점 위치
	FVector TargetLocation;

	// Limits
	float WidthLimit;
	float HeightLimit;
	float Distance;

	UPROPERTY(BlueprintReadOnly)
	FVector BackDirection;

	UPROPERTY(VisibleInstanceOnly)
	TArray<ALanternActor* > NeighborArray;

	UPROPERTY()
	FRotator BaseRotation;
	float CurrentTime = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float TravelTime = 5.f;
	
	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* CapsuleComponent;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UStaticMeshComponent* BlankMeshComponent;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* LanternMeshComponent;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float UpSpeed = 3.f;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float SideSpeed = 1.f;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float BackSpeed = 2.f;

	// 개별 변수
	int32 TypeIndex = 0;
	int32 VariationIndex = 0;

	// NeighborLantern을 저장하기 위해 Indexing하던 변수 (Depricated)
	int32 LanternArrayIndex = 0;
};
