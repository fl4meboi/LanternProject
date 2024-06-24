// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LanternManager.generated.h"

class AMqttManager;
class ACapturer;
class ULanternWidget;
class ALanternActor;

struct FLanternData;

USTRUCT()
struct FLanternType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ALanternActor>> LanternClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ULanternWidget> LanternWidgetClass;
	UPROPERTY()
	ULanternWidget* LanternWidget;
};

UCLASS()
class LANTERNPROJECT_API ALanternManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALanternManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UTexture2D* TextureFromWidget(UUserWidget* const Widget, const FVector2D& DrawSize);

	void SpawnAndDestroyLantern();

	ALanternActor* JustSpawnLantern(UTexture2D* Image, FString Text);
	UFUNCTION(BlueprintCallable)
	void SpawnLantern(UTexture2D* Image, FString Text);
	UFUNCTION(BlueprintCallable)
	void RemoveAllLantern();

	const TArray<ALanternActor*>& GetLanternArray() const;

	void LoadLantern(TArray<FLanternData>& LanternDataArray);

	void SetLimitLocation(FVector NewUpLimitLocation, FVector NewRightLimitLocation, FVector NewTargetLocation);
	void SetSpawnLocation(FVector NewSpawnLocation);

private:
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess))
	ACapturer* Capturer;
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess))
	AMqttManager* MqttManager;
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess))
	bool bIsSpawning = true;
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess))
	bool bIsLoading = false;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess))
	float StartSpawnDelay = 20.f;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess))	
	float SpawnCoolDown = 2.f;
	float CurrentSpawnCoolDown = 0.f;

	// Speed control
	UPROPERTY()
	class ULanternGameInstance* GameInstance;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess))
	float BackSpeed = 2.f;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess))
	float UpSpeed = 10.f;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess))
	float SideSpeed = 1.f;

	UPROPERTY()
	TArray<ALanternActor*> LanternArray;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess, MakeEditWidget))
	FVector TargetLocation;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess, MakeEditWidget))
	FVector UpLimitLocation;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess, MakeEditWidget))
	FVector RightLimitLocation;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess, MakeEditWidget))
	FVector SpawnLocation;

	FVector CameraLocation;

	float WidthLimit;
	float HeightLimit;
	float Distance;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess))
	TArray<FLanternType> LanternTypeArray;

	// Widget
	UPROPERTY(EditInstanceOnly, Meta = (AllowPrivateAccess))
	int32 WidgetType = -1;
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess))
	FVector2D WidgetSize = {2048, 2048};

	// Sound
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess))
	USoundBase* LanternSpawnSound;
};
