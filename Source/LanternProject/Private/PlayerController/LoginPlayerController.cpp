// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/LoginPlayerController.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "GameInstance/LanternGameInstance.h"
#include "LanternProject/LanternProject.h"
#include "Widget/LoginWidget.h"

void ALoginPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);

	Login();
}

void ALoginPlayerController::Login()
{
	ULanternGameInstance* GameInstance = GetGameInstance<ULanternGameInstance>();

	check(GameInstance);
	const TMap<FString, FString>& ConfigMap = GameInstance->GetConfigMap();
	if (ConfigMap.IsEmpty())
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Failed to load config file.")));
		GameInstance->ShowAlertMessage(TEXT("설정 파일 읽기 실패"));
		return;
	}

	
	FString URL = *ConfigMap.Find(FString("license_url")) + FString("?app_id=") +
		*ConfigMap.Find(FString("app_id")) + FString("&license=") + *ConfigMap.Find(FString("license")); 

	// UE_LOG(LogTemp, Warning, TEXT("UBalloonGameInstance::Login) URL : %s"), *URL);
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb("Get");
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ALoginPlayerController::OnLoginResponseReceived);
	HttpRequest->ProcessRequest();

	//
	//
	// FString PostURL = *ConfigMap.Find(FString("api_url")) + FString("/api/interactive/callback.do");
	// TSharedRef<IHttpRequest, ESPMode::ThreadSafe> PostHttpRequest = FHttpModule::Get().CreateRequest();
	// PostHttpRequest->SetURL(URL);
	// PostHttpRequest->SetVerb("Post");
	// PostHttpRequest->
	//
	//
	// PostHttpRequest->OnProcessRequestComplete().BindUObject(this, &ALoginPlayerController::OnLoginResponseReceived);
	// PostHttpRequest->ProcessRequest();
}

void ALoginPlayerController::OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bConnectedSuccessfully)
{
	TSharedPtr<FJsonObject> ResponseObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	FJsonSerializer::Deserialize(Reader, ResponseObject);

	FString Result = ResponseObject->GetStringField(FString("result"));
	// UE_LOG(LogTemp, Warning, TEXT("UBalloonGameInstance::OnLoginResponseReceived) Result : %s"), *Result);

	ULanternGameInstance* GameInstance = GetGameInstance<ULanternGameInstance>();
	check(GameInstance);

	// Todo : Current Logic is when work result is false, Should Change it to work when result is true   
	if (Result.Compare(FString("true")) == 0)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Success to verification license")));
		this->ClientTravel(TravelURL, TRAVEL_Absolute);
	}
	else
	{
		GameInstance->ShowAlertMessage(TEXT("인증에 실패하였습니다."));
		GameInstance->LogToFile(LOGTEXT(TEXT("Fail to verification license")));
	}
}