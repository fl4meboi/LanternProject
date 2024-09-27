// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "HTTP.h"
#include "DTMqtt/DTMqtt/DTMqttClient.h"
#include "GameInstance/LanternGameInstance.h"
#include "LanternProject/LanternProject.h"

#include "MqttManager.generated.h"

class ULanternGameInstance;

class FMqttAsyncTask : public FNonAbandonableTask
{
public:
	FMqttAsyncTask(ULanternGameInstance* NewGameInstance, UDTMqttClient* NewMqttClient, FString NewServerUrl)
		: GameInstance(NewGameInstance), MqttClient(NewMqttClient), ServerUrl(NewServerUrl), ClientId(FGuid::NewGuid().ToString()) {}
	~FMqttAsyncTask() {}

	void DoWork()
	{
		while (true)
		{
			// if (MqttClient->IsValidLowLevelFast() == false || MqttClient->MqttClient->is_connected()) return;
			
			MqttClient->Connect(ServerUrl, ClientId,FString("ychahn"), FString("1"), 60, bIsSuccess, ErrorMsg);

			if (bIsSuccess)
			{
				AsyncTask(ENamedThreads::GameThread, [this]()
				{
					if (GameInstance->IsValidLowLevelFast())
					{
						GameInstance->LogToFile(LOGTEXT(TEXT("Success to connect")));
					}
				});
				return;
			}
			else
			{
				AsyncTask(ENamedThreads::GameThread, [this]()
				{
					if (GameInstance->IsValidLowLevelFast())
					{
						GameInstance->ShowToastMessage(TEXT("연결 실패"));
						GameInstance->LogToFile(LOGTEXT(TEXT("Failed to connect")));	
					}
				});
				FPlatformProcess::Sleep(3.0f);
			}
		}
	}
	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FMqttAsyncTask, STATGROUP_ThreadPoolAsyncTasks)}

private:
	ULanternGameInstance* GameInstance;

	UDTMqttClient* MqttClient;
	FString ServerUrl;
	FString ClientId;

	bool bIsSuccess = false;
	FString ErrorMsg;
	int32 LoopCount = 10;
};

class ALanternManager;

UCLASS()
class LANTERNPROJECT_API AMqttManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMqttManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	const TArray<FString>& GetMqttMessageArray() const;

	// Load function
	void LoadMqttMessageArray(const TArray<FString>& NewMqttMessageArray);

	UFUNCTION(BlueprintCallable)
	void CreateMqttClient();
	UFUNCTION(BlueprintCallable)
	void ConnectToServer();
	UFUNCTION(BlueprintCallable)
	void ReconnectToServer();

	// Delegate function
	UFUNCTION()
	void OnConnected(const FString& Cause);
	UFUNCTION()
	void OnDisconnected(const FString& Cause);
	UFUNCTION(BlueprintCallable)
	void OnMessage(const FString& Topic, const FString& Message);

	bool CheckIsLanternData(const FString& Message);
	UFUNCTION()
	bool ParseMessage(const FString& Message);

	UFUNCTION()
	bool RequestHTTP(const FString& URL);

	UFUNCTION()
	void CallbackHTTP(const FString& Base64, const FString& ImageFormat);

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bIsConnectedSuccessfully);

	void OnCallbackResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bIsConnectedSuccessfully);

	UTexture2D* Base64ToTexture2D(const FString& Base64String, const FString& ImageFormatString);

	bool HasMessage() const;

	void RequestLanternData();

	TArray<uint8> FStringToUint8(const FString& InString);

	const FString& GetPId() const;

private:
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess))
	TArray<FString> MqttMessageArray;
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess))
	ALanternManager* LanternManager;
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess))
	TMap<FString, FString> ConfigMap;
	UPROPERTY()
	UDTMqttClient* MqttClient;

	// Load
	bool bIsLoading = false;

	// Reconnect
	FTimerHandle ReconnectTimerHandle;

	// Data
	FString PId;
	FString Text;
	FString ImageURL;
	FString CallbackURL;

	// Mutex
	mutable  FCriticalSection DataGuard;

	// GameInstance
	class ULanternGameInstance* GameInstance;
};
