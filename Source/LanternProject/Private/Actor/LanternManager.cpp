// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/LanternManager.h"

#include "EngineUtils.h"
#include "LanternActor.h"
#include "Actor/Capturer.h"
#include "Blueprint/UserWidget.h"
#include "Actor/MqttManager.h"
#include "SaveGame/LanternSaveGame.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameInstance/LanternGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "LanternProject/LanternProject.h"
#include "Slate/WidgetRenderer.h"
#include "Widget/LanternWidget.h"
#include "TimerManager.h"

// Sets default values
ALanternManager::ALanternManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALanternManager::BeginPlay()
{
	Super::BeginPlay();

	for (FLanternType& LanternType : LanternTypeArray)
	{
		LanternType.LanternWidget = CreateWidget<ULanternWidget>(GetWorld()->GetFirstPlayerController(), LanternType.LanternWidgetClass);
	}

	GameInstance = GetGameInstance<ULanternGameInstance>();
	check(GameInstance);

	// Set Config data

	const TMap<FString, FString>& ConfigMap = GameInstance->GetConfigMap();

	BackSpeed = FCString::Atof(** ConfigMap.Find(FString("backspeed")));
	UpSpeed = FCString::Atof(** ConfigMap.Find(FString("upspeed")));
	SideSpeed = FCString::Atof(** ConfigMap.Find(FString("sidespeed")));

	// Set spawn data

	HeightLimit = UpLimitLocation.Z - TargetLocation.Z;
	WidthLimit = RightLimitLocation.Y - TargetLocation.Y;
	Distance = FMath::Abs(TargetLocation.X);

	CameraLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	SpawnAndDestroyLantern();

	// Get MqttManager
	TActorIterator<AMqttManager> It(GetWorld());
	MqttManager = *It;
	check(MqttManager);

	// Get Capturer
	TActorIterator<ACapturer> CapturerIter(GetWorld());
	Capturer = *CapturerIter;
	check(Capturer);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		bIsSpawning = false;
	}), StartSpawnDelay, false);
}

// Called every frame
void ALanternManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsLoading || bIsSpawning) return;

	if (CurrentSpawnCoolDown > 0.f)
	{
		CurrentSpawnCoolDown -= DeltaTime;

		// UE_LOG(LogTemp, Warning, TEXT("SpawnCoolTime set"));
		return;
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("SpawnCoolTime not set"));
	}

	if (MqttManager && MqttManager->HasMessage())
	{
		bIsSpawning = true;

		CurrentSpawnCoolDown = SpawnCoolDown;

		MqttManager->RequestLanternData();
	}
}

UTexture2D* ALanternManager::TextureFromWidget(UUserWidget* const Widget, const FVector2D& DrawSize)
{
	if (FSlateApplication::IsInitialized()
		&& Widget != nullptr && Widget->IsValidLowLevel()
		&& DrawSize.X >= 1 && DrawSize.Y >= 1)
	{
		TSharedPtr<SWidget> SlateWidget(Widget->TakeWidget());
		if (!SlateWidget.IsValid()) return nullptr;;

		TSharedPtr<FWidgetRenderer> WidgetRenderer = MakeShareable(new FWidgetRenderer(true));
		if (!WidgetRenderer.IsValid()) return nullptr;

		UTextureRenderTarget2D* TextureRenderTarget = WidgetRenderer->DrawWidget(SlateWidget.ToSharedRef(), DrawSize);

		UTexture2D* Texture = UTexture2D::CreateTransient(DrawSize.X, DrawSize.Y, PF_B8G8R8A8);

		// Lock and copeis the data between the textures
		TArray<FColor> SurfData;
		FRenderTarget* RenderTarget = TextureRenderTarget->GameThread_GetRenderTargetResource();
		RenderTarget->ReadPixels(SurfData);

		void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		Texture->GetPlatformData()->Mips;

		const int32 TextureDataSize = SurfData.Num() * 4;
		FMemory::Memcpy(TextureData, SurfData.GetData(), TextureDataSize);
		Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
		Texture->UpdateResource();

		// Free resources
		SurfData.Empty();
		TextureRenderTarget->ConditionalBeginDestroy();
		SlateWidget.Reset();
		WidgetRenderer.Reset();

		return Texture;
	}
	return nullptr;
}

void ALanternManager::SpawnAndDestroyLantern()
{
	ALanternActor* Lantern = JustSpawnLantern(nullptr, FString());
	Lantern->Destroy();

	GameInstance->LogToFile(LOGTEXT(TEXT("SpawnAndDestroyLantern")));
}

ALanternActor* ALanternManager::JustSpawnLantern(UTexture2D* Image, FString Text)
{
	UWorld* World = GetWorld();
	check(World);

	int32 TypeIndex = FMath::RandRange(0, LanternTypeArray.Num() - 1);
	int32 VariationIndex = FMath::RandRange(0, LanternTypeArray[TypeIndex].LanternClass.Num() - 1);

	LanternTypeArray[TypeIndex].LanternWidget->InitWidget(Image, Text);
	GameInstance->LogToFile(LOGTEXT(TEXT("LanternManager::JustSpawnLantern->InitWidget")));		// Call InitWidget 

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ALanternActor* Lantern = World->SpawnActor<ALanternActor>(LanternTypeArray[TypeIndex].LanternClass[VariationIndex],
		SpawnLocation, GetActorRotation() + FRotator(0, 180, 0), Params);

	FVector BackDirection =
	TargetLocation +
		(GetActorRightVector() * WidthLimit * FMath::RandRange(-1.f, 1.f)) +
		(GetActorUpVector() * HeightLimit * FMath::RandRange(-1.f, 1.f));

	Lantern->Init(TextureFromWidget(LanternTypeArray[TypeIndex].LanternWidget, WidgetSize), BackDirection.GetSafeNormal(), TypeIndex, VariationIndex);
	Lantern->SetData(CameraLocation, SpawnLocation, GetActorLocation() + TargetLocation, WidthLimit, HeightLimit, Distance);
	Lantern->SetLanternSpeed(BackSpeed, UpSpeed, SideSpeed);

	return Lantern;
}

void ALanternManager::SpawnLantern(UTexture2D* Image, FString Text)
{
	if (ALanternActor* Lantern = JustSpawnLantern(Image, Text))
	{
		if (LanternArray.Num() == 500)
		{
			LanternArray[0]->Destroy();
			LanternArray.RemoveAt(0);
		}

		LanternArray.Add(Lantern);

		Capturer->Capture(Lantern);
	
		if (LanternSpawnSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), LanternSpawnSound);
		}
	}

	GameInstance->LogToFile(LOGTEXT(TEXT("Lantern successfully spawned")));

	bIsSpawning = false;
}

void ALanternManager::RemoveAllLantern()
{
	for (int i = 0; i < LanternArray.Num(); ++i)
	{
		LanternArray[i]->Destroy();
	}
	LanternArray.Empty();

	GameInstance->LogToFile(LOGTEXT(TEXT("Successfylly removed all lanterns")));
}

const TArray<ALanternActor*>& ALanternManager::GetLanternArray() const
{
	return LanternArray;
}

void ALanternManager::LoadLantern(TArray<FLanternData>& LanternDataArray)
{
	UWorld* World = GetWorld();
	check(World);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (auto LanternData : LanternDataArray)
	{
		ALanternActor* Lantern = World->SpawnActor<ALanternActor>(LanternTypeArray[LanternData.TypeIndex].LanternClass[LanternData.VariationIndex],
		GetActorLocation(), GetActorRotation() + FRotator(0, 180, 0), Params);

		// Todo : Load Image From Data
		UTexture2D* NewTexture = UTexture2D::CreateTransient(LanternData.TextureSize.X, LanternData.TextureSize.Y, PF_B8G8R8A8);
		NewTexture->UpdateResource();

		FTexture2DMipMap& Mip = NewTexture->GetPlatformData()->Mips[0];
		uint8* MipDataPtr = static_cast<uint8*>(Mip.BulkData.Lock(LOCK_READ_WRITE));
		FMemory::Memcpy(MipDataPtr, LanternData.TextureColors.GetData(), LanternData.TextureSize.X * LanternData.TextureSize.Y * 4);

		// 텍스쳐 리소스 갱신
		NewTexture->UpdateResource();

		// Lantern Data 갱신
		Lantern->Init(NewTexture, LanternData.BackDirection, LanternData.TypeIndex, LanternData.VariationIndex);
		Lantern->SetData(CameraLocation, GetActorLocation(), GetActorLocation() + TargetLocation, WidthLimit, HeightLimit, Distance);
		Lantern->SetLanternSpeed(BackSpeed, UpSpeed, SideSpeed);
		Lantern->SetActorLocation(LanternData.Transform.GetLocation(), true);

		LanternArray.Add(Lantern);
	}

	for (int i = 0; i < LanternArray.Num(); ++i)
	{
		for (int32 LanternIndex :LanternDataArray[i].OverlappedLanternIndexArray)
		{
			LanternArray[i]->AddNeighborLantern(LanternArray[LanternIndex]);
		}
	}

	bIsLoading = false;
}

void ALanternManager::SetLimitLocation(FVector NewUpLimitLocation, FVector NewRightLimitLocation,
	FVector NewTargetLocation)
{
	UpLimitLocation = NewUpLimitLocation;
	RightLimitLocation = NewRightLimitLocation;
	TargetLocation = NewTargetLocation;
}

void ALanternManager::SetSpawnLocation(FVector NewSpawnLocation)
{
	SpawnLocation = NewSpawnLocation;
}


