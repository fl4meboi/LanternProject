// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Capturer.h"

#include "EngineUtils.h"
#include "ImageUtils.h"
#include "Actor/MqttManager.h"
#include "GameInstance/LanternGameInstance.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Compression/OodleDataCompressionUtil.h"
#include "Engine/TextureRenderTarget2D.h"

// Sets default values
ACapturer::ACapturer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent2D"));
	SetRootComponent(SceneCaptureComponent2D);
	SceneCaptureComponent2D->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCaptureComponent2D->bCaptureEveryFrame = false;
}

// Called when the game starts or when spawned
void ACapturer::BeginPlay()
{
	Super::BeginPlay();

	TActorIterator<AMqttManager> It(GetWorld());
	MqttManager = *It;
	check(MqttManager);
	
	// UE_LOG(LogTemp, Warning, TEXT("Capturer called"));
	GameInstance->LogToFile(LOGTEXT(TEXT("Capturer called")));
}

// Called every frame
void ACapturer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UTexture2D* ACapturer::TestCapture_Implementation(AActor* Target)
{
	SceneCaptureComponent2D->ShowOnlyActors.Add(Target);
	SetActorLocation(Target->GetActorLocation() + FVector(-430, 0, 160));
	SceneCaptureComponent2D->CaptureScene();
	SceneCaptureComponent2D->ShowOnlyActors.Remove(Target);
	FString Base64 = ConvertTextureToBase64(SceneCaptureComponent2D->TextureTarget, TEXT("jpeg"));
	// UE_LOG(LogTemp, Warning, TEXT("Capture jpeg"));

	TActorIterator<AMqttManager> It(GetWorld());

	return (*It)->Base64ToTexture2D(Base64, TEXT("JPG"));

	GameInstance->LogToFile(LOGTEXT(TEXT("Test Capture implemented")));
}

void ACapturer::Capture(AActor* Target)
{
	// 여기서 이미지 포맷 결정, 사용 가능한 포맷 : jpg, png
	FString ImageFormat = TEXT("jpg");
	SceneCaptureComponent2D->ShowOnlyActors.Add(Target);
	SetActorLocation(Target->GetActorLocation() + FVector(-430, 0, 160));
	SceneCaptureComponent2D->CaptureScene();
	SceneCaptureComponent2D->ShowOnlyActors.Remove(Target);
	FString Base64 = ConvertTextureToBase64(SceneCaptureComponent2D->TextureTarget, ImageFormat);
	// UE_LOG(LogTemp, Warning, TEXT("Capturer jpg"));

	MqttManager->CallbackHTTP(Base64, ImageFormat);

	GameInstance->LogToFile(LOGTEXT(TEXT("Test Capture implemented")));	
}

FString ACapturer::ConvertTextureToBase64(UTextureRenderTarget2D* Texture, const FString& ImageFormat)
{
	if (Texture == nullptr)
	{
		return FString();
	}

	// TArray64<uint8> OutArray;
	// FImageUtils::GetRawData(Texture, OutArray);
	// return FBase64::Encode(OutArray.GetData(), OutArray.Num());
	
	FImage Image;
	FImageUtils::GetRenderTargetImage(Texture,Image);
	TArray64<uint8> CompressedData;
	FImageUtils::CompressImage(CompressedData,*ImageFormat,Image);

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, CompressedData, ImageFormat]()
	{
		if (ULanternGameInstance* GameInstasnce =  GetGameInstance<ULanternGameInstance>())
		{
			IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

			FDateTime Now = FDateTime::Now();

			FString FolderName = FString::FromInt(Now.GetYear());
			if (Now.GetMonth() < 10)
			{
				FolderName += TEXT("0");
			}
			FolderName += FString::FromInt(Now.GetMonth());
			if (Now.GetDay() < 10)
			{
				FolderName += TEXT("0");
			}
			FolderName += FString::FromInt(Now.GetDay());
		
			// UE_LOG(LogTemp, Warning, TEXT("ACapturer::ConvertTextureToBase64) %s"), *);
		
			FString ImagePath = GameInstasnce->GetFilePath() + TEXT("Screenshots/") + FolderName + TEXT("/");

			// Check Directory
			PlatformFile.CreateDirectoryTree(*ImagePath);
		
			FString ImageName = MqttManager->GetPId() + TEXT(".") + ImageFormat;
		
			FFileHelper::SaveArrayToFile(CompressedData, *(ImagePath + ImageName));
		}
	});

	FString Base64EncodeData = FBase64::Encode(CompressedData.GetData(), CompressedData.Num());

	return Base64EncodeData;
}

