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

	void VerticalTextSet(const FString& Text, UTextBlock* TextBox1, UTextBlock* TextBox2 = nullptr, UTextBlock* TextBox3 = nullptr, UTextBlock* TextBox4 = nullptr);

	void HideWidget();

	void HideText();

private:
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UImage* Img_Image;
	// UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	// UImage* Img_Image2;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UTextBlock* TB_Text_1;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UTextBlock* TB_Text_2;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UTextBlock* TB_Text_3;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UTextBlock* TB_Text_4;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UTextBlock* TB_Text_5;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UTextBlock* TB_Text_6;

	// UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	// UTextBlock* TB_Text2;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UScaleBox* ScaleBox;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
	UWidgetSwitcher* WidgetSwitcher;
	
	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* HideWidgetAnim;

	// UPROPERTY(EditAnywhere, Category = "Lantern")
	// float ShowDuration = 1.0f;
};
