// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/IHttpRequest.h"
#include "LoginPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LANTERNPROJECT_API ALoginPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Login();
	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

private:
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess))
	FString TravelURL;
};
