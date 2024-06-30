// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LanternGameModeBase.generated.h"

USTRUCT(BlueprintType)
struct FPawnType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APawn> PawnClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector UpLimitLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RightLimitLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SpawnLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* PlayerStart;
};
/**
 * 
 */
UCLASS()
class LANTERNPROJECT_API ALanternGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	UPROPERTY()
	class ULanternGameInstance* GameInstance;
	UPROPERTY(VisibleInstanceOnly, Meta = (AllowPrivateAccess))
	int32 PawnTypeIndex = -1;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess))
	TArray<FPawnType> PawnTypeArray;
};
