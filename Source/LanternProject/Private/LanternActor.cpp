// Fill out your copyright notice in the Description page of Project Settings.


#include "LanternActor.h"

#include "Blueprint/UserWidget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Slate/WidgetRenderer.h"

// Sets default values
ALanternActor::ALanternActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
	LanternMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("LanternMesh");
	LanternMeshComponent->SetupAttachment(RootComponent);
	LanternMeshComponent->SetCollisionProfileName(FName("OverlapAllDynamic"));
	BlankMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("BlankMeshComponent");
	BlankMeshComponent->SetupAttachment(RootComponent);
	BlankMeshComponent->SetRenderCustomDepth(true);
}

UTexture2D* ALanternActor::TextureFromWidget(UUserWidget* const Widget, const FVector2D& DrawSize)
{
	if (FSlateApplication::IsInitialized()
		&& Widget != nullptr && Widget->IsValidLowLevel()
		&& DrawSize.X >= 1 && DrawSize.Y >= 1)
	{
		TSharedPtr<SWidget> SlateWidget(Widget->TakeWidget());
		if (!SlateWidget.IsValid()) return nullptr;

		TSharedPtr<FWidgetRenderer> WidgetRenderer = MakeShareable(new FWidgetRenderer(true));
		if (!WidgetRenderer.IsValid()) return nullptr;

		UTextureRenderTarget2D* TextureRenderTarget = WidgetRenderer->DrawWidget(SlateWidget.ToSharedRef(), DrawSize);

		UTexture2D* Texture = UTexture2D::CreateTransient(DrawSize.X, DrawSize.Y, PF_B8G8R8A8);

		// Lock and copies the data between the textures
		TArray<FColor> SurfData;
		FRenderTarget* RenderTarget = TextureRenderTarget->GameThread_GetRenderTargetResource();
		RenderTarget->ReadPixels(SurfData);

		void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

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

// Called when the game starts or when spawned
void ALanternActor::BeginPlay()
{
	Super::BeginPlay();

	LanternMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ALanternActor::OnLanternMeshBeginOverlap);
	LanternMeshComponent->OnComponentEndOverlap.AddDynamic(this, &ALanternActor::OnLanternMeshEndOverlap);
	SetActorTickEnabled(false);
}

// Called every frame
void ALanternActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorRotation(FRotator(0, (CameraLocation - GetActorLocation()).Rotation().Yaw, 0));

	FVector ActorLocation = GetActorLocation();
	int32 NeighborCount = NeighborArray.Num();

	// Back
	float CurrentDistance = FMath::Abs(ActorLocation.X - BaseLocation.X);

	// 현재 거리비
	float DistanceAlpha = CurrentDistance/Distance;
	// Center Lerp
	FVector CurrentCenter = FMath::Lerp(BaseLocation, TargetLocation, DistanceAlpha);
	// 현재 최대 좌우 거리
	float CurrentWidthLimit = FMath::Lerp(0, WidthLimit, DistanceAlpha);
	// 현재 최대 상하 거리
	float CurrentHeightLimit = FMath::Lerp(0, HeightLimit, DistanceAlpha);

	if (CurrentDistance <= Distance)
	{
		AddActorWorldOffset(FVector(BackDirection.X, 0, 0) * BackSpeed * DeltaTime);
	}
	if (FMath::Abs(ActorLocation.Y - BaseLocation.Y) < CurrentWidthLimit)
	{
		AddActorWorldOffset(FVector(0, BackDirection.Y, 0) * SideSpeed * DeltaTime);
	}

	FVector Location = GetActorLocation();

	Location.Z = FMath::Lerp(BaseLocation.Z, (BaseLocation + (BackDirection * Distance)).Z, DistanceAlpha);
	SetActorLocation(Location);

	AddActorWorldOffset(FVector(0, 0, BackDirection.Z) * UpSpeed * DeltaTime);

	if (NeighborCount > 0)
	{
		NeighborCount++;
		FVector Separation = GetSeparation();

		float PowedNeighborCount = NeighborCount * NeighborCount;

		AddActorWorldOffset(FVector(Separation.X, 0, 0) * BackSpeed * PowedNeighborCount * DeltaTime);

		// Side
		if (FMath::Abs(ActorLocation.Y - BaseLocation.Y) < WidthLimit)
		{
			AddActorWorldOffset(FVector(0, Separation.Y, 0) * SideSpeed * PowedNeighborCount * DeltaTime);
		}
	}
	
	// FVector UpwardMovement = FVector(0.f, 0.f, UpSpeed * DeltaTime);
	//
	// AddActorWorldOffset(UpwardMovement, true);
}

void ALanternActor::SetLanternSpeed(float NewBackSpeed, float NewUpSpeed, float NewSideSpeed)
{
	if (NewBackSpeed != -1) BackSpeed = NewBackSpeed;
	if (NewUpSpeed != -1) UpSpeed = NewUpSpeed;
	if (NewSideSpeed != -1) SideSpeed = NewSideSpeed;
}

void ALanternActor::Init(UTexture2D* LanternImage, const FVector& NewBackDirection, int32 NewTypeIndex,
	int32 NewVariationIndex)
{
	if (UMaterialInstanceDynamic* InstanceDynamic = BlankMeshComponent->CreateDynamicMaterialInstance(0))
	{
		BackDirection = NewBackDirection;
		InstanceDynamic->SetTextureParameterValue(FName("Texture"), LanternImage);
	}

	TypeIndex = NewTypeIndex;
	VariationIndex = NewVariationIndex;
}

void ALanternActor::SetData(const FVector& NewCameraLocation, const FVector& NewBaseLocation,
	const FVector NewTargetLocation, float NewWidthLimit, float NewHeightLimit, float NewDistance)
{
	CameraLocation = NewCameraLocation;
	BaseLocation = NewBaseLocation;
	TargetLocation = NewTargetLocation;
	WidthLimit = NewWidthLimit;
	HeightLimit = NewHeightLimit;
	Distance = NewDistance;

	SetActorTickEnabled(true);
}

void ALanternActor::OnLanternMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ALanternActor* OtherLantern = Cast<ALanternActor>(OtherActor);
	if (OtherLantern == nullptr) return;
	if (NeighborArray.Contains(OtherLantern) == false && OtherLantern->GetActorLocation().X < GetActorLocation().X)
	{
		NeighborArray.Add(OtherLantern);
		if (IsActorTickEnabled() == false)
		{
			SetActorTickEnabled(true);
		}
	}
}

void ALanternActor::OnLanternMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ALanternActor* OtherLantern = Cast<ALanternActor>(OtherActor);
	if (NeighborArray.Contains(OtherLantern) == true)
	{
		NeighborArray.Remove(OtherLantern);
	}
}

FVector ALanternActor::GetSeparation()
{
	FVector Separation = FVector::ZeroVector;
	for (auto Actor : NeighborArray)
	{
		FVector Current = GetActorLocation() - Actor->GetActorLocation();
		Current /= Current.Length();
		Separation += Current;
	}

	Separation /= NeighborArray.Num();
	
	return Separation;
}

void ALanternActor::AddNeighborLantern(ALanternActor* NewNeighborLantern)
{
	NeighborArray.Add(NewNeighborLantern);
}

void ALanternActor::SetLanternArrayIndex(const TArray<ALanternActor*>& LanternArray)
{
	LanternArrayIndex = LanternArray.Find(this); 
}

int32 ALanternActor::GetLanternArrayIndex() const
{
	return LanternArrayIndex;
}

void ALanternActor::SaveData(FLanternData& LanternData)
{
	// UMaterialInstanceDynamic* Material = Cast<UMaterialInstanceDynamic>(BlankMeshComponent->GetMaterial(0));
	// FHashedMaterialParameterInfo Info(FName("Texture"));
	// UTexture* OutTexture;
	// // if (Material->GetTextureParameterValue(Info, OutTexture))
	// // {
	// // 	if (UTexture2D* Texture = Cast<UTexture2D>(OutTexture))
	// // 	{
	// // 		FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	// // 		LanternData.TextureSize.X = Mip.SizeX;
	// // 		LanternData.TextureSize.Y = Mip.SizeY;
	// // 		int32 DataLength = LanternData.TextureSize.X * LanternData.TextureSize.Y * 4;	// Assume RGBA format
	// // 		LanternData.TextureColors.AddUninitialized(DataLength);
	// //
	// // 		const uint8* MipDataPtr = static_cast<const uint8*>(Mip.BulkData.LockReadOnly());
	// // 		FMemory::Memcpy(LanternData.TextureColors.GetData(), MipDataPtr, DataLength);
	// // 		Mip.BulkData.Unlock();
	// // 	}
	// // }
	// //
	// // for (ALanternActor* Lantern : NeighborArray)
	// // {
	// // 	LanternData.OverlappedLanternIndexArray.Add(Lantern->GetLanternArrayIndex());
	// // }
	// //
	// // LanternData.
}

