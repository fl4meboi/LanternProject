// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/MqttManager.h"

#include "EngineUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Actor/LanternManager.h"
#include "SaveGame/LanternSaveGame.h"

// Sets default values
AMqttManager::AMqttManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMqttManager::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = GetGameInstance<ULanternGameInstance>();
	check(GameInstance);
	ConfigMap = GameInstance->GetConfigMap();

	CreateMqttClient();
	ConnectToServer();

	TActorIterator<ALanternManager> It(GetWorld());
	LanternManager = *It;
	check(LanternManager);
}

void AMqttManager::LoadMqttMessageArray(const TArray<FString>& NewMqttMessageArray)
{
	FScopeLock Lock(&DataGuard);

	MqttMessageArray = NewMqttMessageArray;

	bIsLoading = false;
}

// Called every frame
void AMqttManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const TArray<FString>& AMqttManager::GetMqttMessageArray() const
{
	return MqttMessageArray;
}

void AMqttManager::CreateMqttClient()
{
	UDTMqttClient::CreateMqttClient(MqttClient);
	if (MqttClient->IsValidLowLevelFast() == false)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Failed to Create Client")));
		GameInstance->ShowAlertMessage(TEXT("클라이언트 생성에 실패했습니다."));
		return;
	}
	MqttClient->OnConnected.AddDynamic(this, &AMqttManager::OnConnected);
	MqttClient->OnConnectionLost.AddDynamic(this, &AMqttManager::OnDisconnected);
	MqttClient->OnMessageArrived.AddDynamic(this, &AMqttManager::OnMessage);
}

void AMqttManager::ConnectToServer()
{
	FString ServerURL = *ConfigMap.Find(FString("mqtt_broker_ip")) + FString(":") + *ConfigMap.Find(FString("mqtt_broker_port"));
	FString ErrorMsg;
	bool bIsSuccess;

	(new FAutoDeleteAsyncTask<FMqttAsyncTask>(GameInstance, MqttClient, ServerURL))->StartBackgroundTask();

	return;

	MqttClient->Connect(ServerURL, FGuid::NewGuid().ToString(), FString("ychahn"), FString("1"), 60, bIsSuccess, ErrorMsg);
	if (bIsSuccess)
	{
		if (ReconnectTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(ReconnectTimerHandle);
		}

		GameInstance->LogToFile(LOGTEXT(TEXT("Connection with Mqtt broker was successful")));
	}
	else
	{
		GameInstance->ShowToastMessage((TEXT("연결 실패")));
		GameInstance->LogToFile(LOGTEXT(TEXT("AMqttManager::ConnectToServer) Connection to Mqtt broker failed \nError Message : %s"), *ErrorMsg));

		ReconnectToServer();
	}
}

void AMqttManager::ReconnectToServer()
{
	if (ReconnectTimerHandle.IsValid() == false)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Try to reconnect to mqtt broker server")));
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(ReconnectTimerHandle, FTimerDelegate::CreateLambda([this]()
			{
				AsyncTask(ENamedThreads::GameThread, [this]()
				{
					ConnectToServer();
				});
			}), 3.f, true);
	}
}

void AMqttManager::OnConnected(const FString& Cause)
{
	GameInstance->LogToFile(LOGTEXT(TEXT("Cause : %s"), *Cause));

	bool bIsSuccess = false;
	FString ErrorMsg;
	MqttClient->Subscribe(*ConfigMap.Find(FString("mqtt_broker_topic")), EDT_QualityOfService::QoS2, bIsSuccess, ErrorMsg);
	if (bIsSuccess)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Success to subscribe %s topic"), **ConfigMap.Find(FString("mqtt_broker_topic"))));
	}
	else
	{
		GameInstance->ShowToastMessage(TEXT("구독 실패"));
		GameInstance->LogToFile(LOGTEXT(TEXT("Fail to subscribe %s topic.\nError Message : %s"), **ConfigMap.Find(FString("mqtt_broker_topic")), *ErrorMsg));
	}
}

void AMqttManager::OnDisconnected(const FString& Cause)
{
	GameInstance->ShowToastMessage(TEXT("연결 끊김"));
	GameInstance->LogToFile(LOGTEXT(TEXT("Cause : %s"), *Cause));

	bool bIsSuccessToDisconnect = false;
	FString DisconnectErrorMessage;
	MqttClient->Disconnect(bIsSuccessToDisconnect, DisconnectErrorMessage);
	if (bIsSuccessToDisconnect)
	{
		UE_LOG(LogTemp, Warning, TEXT("AmqttManager::OnDisconnected) Success to disconnect"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AmqttManager::OnDisconnected) Failed to disconnect : %s"), *DisconnectErrorMessage);
	}

	ConnectToServer();
}

void AMqttManager::OnMessage(const FString& Topic, const FString& Message)
{
	FScopeLock Lock(&DataGuard);
	if (CheckIsLanternData(Message) == false)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("%s is not LanterData Message"), *Message));
		return;
	}
	GameInstance->LogToFile(LOGTEXT(TEXT("\n				Topic:%s\n				Message:%s"), *Topic, *Message));
	MqttMessageArray.Add(Message);
}

bool AMqttManager::CheckIsLanternData(const FString& Message)
{
	TSharedPtr<FJsonObject> MessageObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);
	FJsonSerializer::Deserialize(Reader, MessageObject);

	if (MessageObject.IsValid() == false)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Message is not Json type")));
		return false;
	}

	TSharedPtr<FJsonObject> DataObject = MessageObject->GetObjectField(TEXT("data"));
	if (DataObject.IsValid())
	{
		TSharedPtr<FJsonObject> HeadObject = DataObject->GetObjectField(TEXT("data"));
		if (HeadObject.IsValid())
		{
			if (HeadObject->GetStringField(TEXT("apicommand")).Compare(TEXT("content")) != 0
				|| HeadObject->GetStringField(TEXT("apiaction")).Compare(TEXT("letter")) != 0
				|| HeadObject->GetStringField(TEXT("apimethod")).Compare(TEXT("push")) != 0)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
	return true;
}

bool AMqttManager::ParseMessage(const FString& Message)
{
	PId = FString();
	Text = FString();
	ImageURL = FString();
	CallbackURL = FString();

	TSharedPtr<FJsonObject> MessageObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);
	FJsonSerializer::Deserialize(Reader, MessageObject);

	if (MessageObject.IsValid() == false)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Message is not Json type")));
		return false;
	}

	TSharedPtr<FJsonObject> DataObject = MessageObject->GetObjectField(TEXT("data"));
	if (DataObject.IsValid())
	{
		TSharedPtr<FJsonObject> HeadObject = DataObject->GetObjectField(TEXT("head"));
		if (HeadObject.IsValid())
		{
			if (HeadObject->GetStringField(TEXT("apicommand")).Compare(TEXT("content")) != 0
			|| HeadObject->GetStringField(TEXT("apiaction")).Compare(TEXT("letter")) != 0
			|| HeadObject->GetStringField(TEXT("apimethod")).Compare(TEXT("push")) != 0)
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		TSharedPtr<FJsonObject> BodyObject = DataObject->GetObjectField(TEXT("body"));
		if (BodyObject.IsValid())
		{
			PId = BodyObject->GetStringField(TEXT("pid"));
			Text = BodyObject->GetStringField(TEXT("text"));
			ImageURL = BodyObject->GetStringField(TEXT("img"));
			CallbackURL = BodyObject->GetStringField(TEXT("callback"));
		}
	}

	return true;
	
	FString LeftStr;
	FString RightStr;
	if (Message.Split(FString("pid\":\""), &LeftStr, &RightStr))
	{
		if (RightStr.Split(FString("\""), &PId, &RightStr))
		{
			if (Message.Split(FString("text\":\""), &LeftStr, &RightStr))
			{
				// UE_LOG(LogTemp, Warning, TEXT("AMqttManager::OnMessage) Found Right String : %s"), *RightStr);
		
				if (RightStr.Split(FString("\""), &Text, &RightStr))
				{
					// UE_LOG(LogTemp, Warning, TEXT("AMqttManager::OnMessage) Text : %s"), *Text);

					if (RightStr.Split(FString("img\":\""), &LeftStr, &RightStr))
					{
						if (RightStr.Split(FString("\""), &ImageURL, &RightStr))
						{
							// UE_LOG(LogTemp, Warning, TEXT("AMqttManager::OnMessage) ImageURL : %s"), *ImageURL);

							if (RightStr.Split(FString("callback\":\""), &LeftStr, &RightStr))
							{
								if (RightStr.Split(FString("\""), &CallbackURL, &RightStr))
								{
									// UE_LOG(LogTemp, Warning, TEXT("AMqttManager::OnMessage) CallbackURL : %s"), *CallbackURL);
								}
							}
						}
					}
				}
			}
		}
		
	}
}

bool AMqttManager::RequestHTTP(const FString& URL)
{
	if (URL.IsEmpty())
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Image Url is empty")));

		return false;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb("Get");
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AMqttManager::OnResponseReceived);

	return HttpRequest->ProcessRequest();
}

void AMqttManager::CallbackHTTP(const FString& Base64, const FString& ImageFormat)
{
	if (CallbackURL.IsEmpty()) return;
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("POST");

	HttpRequest->SetURL(CallbackURL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	TSharedRef<FJsonObject> RequestObj = MakeShared<FJsonObject>();
	RequestObj->SetStringField("app_id", *ConfigMap.Find(FString("app_id")));
	RequestObj->SetStringField("message_pid", PId);
	RequestObj->SetStringField("data_b64image", TEXT("data:image/") + ImageFormat + TEXT(";base64,") + Base64);
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(RequestObj, Writer);
	HttpRequest->SetContentAsString(RequestBody);

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AMqttManager::OnCallbackResponseReceived);

	HttpRequest->ProcessRequest();
}

void AMqttManager::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bIsConnectedSuccessfully)
{
	if (bIsConnectedSuccessfully == false)
	{
		FString Content = UTF8_TO_TCHAR(Request->GetContent().GetData());
		GameInstance->LogToFile(LOGTEXT(TEXT("Image Http Request was failed\nRequest:%s\nResponse:%s"), *Content, *Response->GetContentAsString()));
		return;
	}

	TSharedPtr<FJsonObject> ResponseObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	FJsonSerializer::Deserialize(Reader, ResponseObject);

	if (ResponseObject->GetStringField(TEXT("status")).Compare(TEXT("00200")) != 0)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Image HTTP Response status is invalid. Status is %s"), *ResponseObject->GetStringField(TEXT("status"))));
		return;
	}

	TSharedPtr<FJsonObject> ResultObject = ResponseObject->GetObjectField(FString("result"));
	if (ResultObject.IsValid())
	{
		FString DataString = ResultObject->GetStringField(FString("data_b64image"));
		FString LeftStr;
		DataString.Split(",", &LeftStr, &DataString);
		FString ImageFormat;
		LeftStr.Split("/", &LeftStr, &ImageFormat);
		ImageFormat.Split(";", &ImageFormat, &LeftStr);

		LanternManager->SpawnLantern(Base64ToTexture2D(DataString, ImageFormat), Text);

		GameInstance->LogToFile(LOGTEXT(TEXT("mqtt requested to spawn lantern with image")));
	}
	else
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Image Http Response has no 'result' field")));
	}
}

void AMqttManager::OnCallbackResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bIsConnectedSuccessfully)
{
	if (bIsConnectedSuccessfully == false)
	{
		FString Content = UTF8_TO_TCHAR(Request->GetContent().GetData());

		GameInstance->LogToFile(LOGTEXT(TEXT("Callback Http Request was failed\nRequest:%s\nResponse:%s"), *Content, *Response->GetContentAsString()));
		return;
	}

	TSharedPtr<FJsonObject> ResponseObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	FJsonSerializer::Deserialize(Reader, ResponseObject);
	if (ResponseObject->GetStringField(TEXT("status")).Compare(TEXT("00200")) != 0)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Callback HTTP Response status is invalid. Status is %s"), *ResponseObject->GetStringField(TEXT("status"))));
	}
}

UTexture2D* AMqttManager::Base64ToTexture2D(const FString& Base64String, const FString& ImageFormatString)
{
	TArray<uint8> DecodedData;
	FBase64::Decode(Base64String, DecodedData);

	// 현재 플랫폼에 적함한 이미지 래퍼 모듈 가져오기
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	EImageFormat ImageFormat;

	if (ImageFormatString.Compare(TEXT("JPG"), ESearchCase::IgnoreCase) == 0 || ImageFormatString.Compare(TEXT("JPEG"), ESearchCase::IgnoreCase) == 0)
	{
		ImageFormat = EImageFormat::JPEG;
	}
	else if (ImageFormatString.Compare(TEXT("PNG"), ESearchCase::IgnoreCase) == 0)
	{
		ImageFormat = EImageFormat::PNG;
	}
	else
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Image is invalid format : %s"), *ImageFormatString));
		return nullptr;
	}

	// 이미지 래퍼 생성 (소스에 따라 PNG, JPG, BMP 등이 될 수 있음)
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
	if (!ImageWrapper.IsValid())
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Cannot create image wrapper")));
		return nullptr;
	}

	// 이미지 데이터 로드
	if (!ImageWrapper->SetCompressed(DecodedData.GetData(), DecodedData.Num()))
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Cannot load compressed image data")));
		return nullptr;
	}

	// 압축 해제된 이미지 데이터 가져오기
	TArray64<uint8> UncompressedData;
	if (!ImageWrapper->GetRaw(UncompressedData))
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Cannot get uncompressed image data")));
		return nullptr;
	}

	// 텍스쳐 생성
	UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
	if (!Texture)
	{
		GameInstance->LogToFile(LOGTEXT(TEXT("Cannot create image texture")));
		return nullptr;
	}

	// 이미지 데이터로 텍스쳐 채우기
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, UncompressedData.GetData(), UncompressedData.Num());
	Mip.BulkData.Unlock();

	// 텍스쳐 설정 업데이트
	Texture->UpdateResource();

	return Texture;
}

bool AMqttManager::HasMessage() const
{
	FScopeLock Lock(&DataGuard);
	return (bIsLoading == false) && (MqttMessageArray.Num() > 0);
}

void AMqttManager::RequestLanternData()
{
	FScopeLock Lock(&DataGuard);

	FString CurrentMessage = MqttMessageArray[0];
	MqttMessageArray.RemoveAt(0);
	
	ParseMessage(CurrentMessage);

	if (ImageURL.IsEmpty() == false)
	{
		RequestHTTP(ImageURL);
	}
	else
	{
		LanternManager->SpawnLantern(nullptr, Text);

		GameInstance->LogToFile(LOGTEXT(TEXT("Mqtt requested to spawn lantern with text")));
		GameInstance->LogToFile(LOGTEXT(TEXT("Mqtt requested to spawn lantern with text")));
	}
}

TArray<uint8> AMqttManager::FStringToUint8(const FString& InString)
{
	TArray<uint8> OutBytes;

	// Handle empty strings
	if (InString.Len() > 0)
	{
		FTCHARToUTF8 Converted(*InString);		// Convert to UTF8
		OutBytes.Append(reinterpret_cast<const uint8*>(Converted.Get()), Converted.Length());
	}
	
	return OutBytes;
}

const FString& AMqttManager::GetPId() const
{
	return PId;
}

