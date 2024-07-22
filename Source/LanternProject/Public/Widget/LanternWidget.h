// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LanternWidget.generated.h"

class UWidgetSwitcher;
class UScaleBox;
class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class LANTERNPROJECT_API ULanternWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitWidget(UTexture2D* Texture, const FString& Text);

	void Randomize(int32 Type = -1);

private:
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UImage* Img_Image;
	// UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	// UImage* Img_Image2;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UTextBlock* TB_Text;
	// UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	// UTextBlock* TB_Text2;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UScaleBox* ScaleBox;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UWidgetSwitcher* WidgetSwitcher;
};
