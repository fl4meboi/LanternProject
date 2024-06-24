// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LanternGameInstance.generated.h"

class AMqttManager;
class UErrorWidget;
class ALanternActor;
class IMqttClientInterface;
class ULanternSaveGame;
class ALanternManager;
/**
 * 
 */
UCLASS()
class LANTERNPROJECT_API ULanternGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;

	virtual void Shutdown() override;

	// Config
	UFUNCTION(BlueprintCallable)
	bool LoadFile(FString FileName, TArray<FString>& FileContent);

	UFUNCTION(BlueprintCallable)
	bool LoadConfig();
	UFUNCTION(BlueprintCallable)
	bool ApplyConfig();

	// Apply config
	UFUNCTION()
	void MoveWindowFunction();

public:
	// Log function
	void OpenLogFile();
	void LogToFile(const FString& InString);
	void MoveToNextFile();

	// Save function
	UFUNCTION(BlueprintCallable)
	void Save();

	UFUNCTION()
	void SaveLanternArray(const TArray<ALanternActor*>& LanternArray);
	UFUNCTION()
	void LoadLanternArray(ALanternManager* LanternManager);

	UFUNCTION()
	void SaveMqttMessageArray(const TArray<FString>& MqttMessageArray);
	UFUNCTION()
	void LoadMqttMessageArray(AMqttManager* MqttManager);

	ULanternSaveGame* GetSaveGame() const;

	// Getter
	const TMap<FString, FString>& GetConfigMap() const;

	const FString& GetFilePath() const;

	// ErrorWidget
	UFUNCTION(BlueprintCallable)
	void ShowAlertMessage(const FString& Message);
	UFUNCTION(BlueprintCallable)
	void ShowToastMessage(const FString& Message);

	UFUNCTION(BlueprintCallable)
	void InitFilePath();

private:
	IFileHandle* FileHandle;
	int32 LogFileIndex = 0;
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess))
	FString SlotName = FString("DefaultSlot");
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (AllowPrivateAccess))
	FString FilePath;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess))
	TMap<FString, FString> ConfigMap;

	UPROPERTY()
	ULanternSaveGame* SaveGame;
	UPROPERTY()
	FTimerHandle MoveWindowTimerHandle;

	// Error Widget
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess))
	TSubclassOf<UErrorWidget> ErrorWidgetClass;
	UPROPERTY()
	UErrorWidget* ErrorWidget;
};
