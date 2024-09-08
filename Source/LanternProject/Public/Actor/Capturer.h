// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Capturer.generated.h"

class AMqttManager;

UCLASS()
class LANTERNPROJECT_API ACapturer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACapturer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UTexture2D* TestCapture(AActor* Target);

	UFUNCTION(BlueprintCallable)
	void Capture(AActor* Target);

	UFUNCTION(BlueprintCallable)
	FString ConvertTextureToBase64(UTextureRenderTarget2D* Texture, const FString& ImageFormat);

	// UPROPERTY()
	// class ULanternGameInstance* GameInstance;

private:
	UPROPERTY()
	AMqttManager* MqttManager;
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess))
	USceneCaptureComponent2D* SceneCaptureComponent2D;

};
