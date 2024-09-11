// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LanternWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
// #include "GameInstance/LanternGameInstance.h"
#include "TimerManager.h"
// #include "LanternProject/LanternProject.h"

void ULanternWidget::InitWidget(UTexture2D* Texture, const FString& Text)
{
	if (nullptr == Texture && Text.IsEmpty())
	{
		Randomize();
		UE_LOG(LogTemp, Warning, TEXT("LanternWidget::InitWidget-Randomize"));
	}
	else
	{
		FString NewText = Text.Replace(TEXT("\\n"), TEXT("\n"));

		if (Texture == nullptr)			// Case 1: no image, up to 32 char in 4 text boxes
		{
			WidgetSwitcher->SetActiveWidgetIndex(1);

			TB_Text_3->SetText(FText::FromString(NewText.Left(8)));		// First 8 char

			if (NewText.Len() > 8)
			{
				TB_Text_4->SetText(FText::FromString(NewText.Mid(8, 8)));	// 9-16 char
			}
			else
			{
				TB_Text_4->SetText(FText::GetEmpty());
			}

			if (NewText.Len() > 16)
			{
				TB_Text_5->SetText(FText::FromString(NewText.Mid(16, 8)));		// 17-24 char
			}
			else
			{
				TB_Text_5->SetText(FText::GetEmpty());
			}

			if (NewText.Len() > 24)
			{
				TB_Text_6->SetText(FText::FromString(NewText.Mid(24, 8)));		// 25-32 char
			}
			else
			{
				TB_Text_6->SetText(FText::GetEmpty());
			}

			UE_LOG(LogTemp, Warning, TEXT("LanternWidget::InitWidget - No Image with up to 32 characters of text"));
		}
		else if (Text.IsEmpty())		// Case 2: upload image, no text
		{
			WidgetSwitcher->SetActiveWidgetIndex(0);
			Img_Image->SetBrushFromTexture(Texture);

			UE_LOG(LogTemp, Warning, TEXT("LanternWidget::InitWidget - No text"));
		}
		else							// Case 3: uploaded image, up to 16 char in 2 text boxes
		{
			WidgetSwitcher->SetActiveWidgetIndex(0);
			Img_Image->SetBrushFromTexture(Texture);

			TB_Text_1->SetText(FText::FromString(NewText.Left(8)));		// First 8 char

			if (NewText.Len() > 8)
			{
				TB_Text_2->SetText(FText::FromString(NewText.Mid(8, 8)));	// Next 8 char
			}
			else
			{
				TB_Text_2->SetText(FText::GetEmpty());
			}

			UE_LOG(LogTemp, Warning, TEXT("LanternWidget::InitWidget - Image with up to 16 characters of text"));
		}
	}
}

void ULanternWidget::Randomize(int32 Type)
{
	if (Type == -1)
	{
		Type = FMath::RandRange(0, 1);
	}

	switch(Type)
	{
	case 0:
		WidgetSwitcher->SetActiveWidgetIndex(0);		// Text + Image
		break;
		
	case 1:
		WidgetSwitcher->SetActiveWidgetIndex(1);		// Text + Text
		break;

	/*	// Image Only
	case 2:
		WidgetSwitcher->SetActiveWidgetIndex(2);
		break;*/
	}
}

void ULanternWidget::HideWidget()
{
	PlayAnimation(HideWidgetAnim);

	// if (TB_Text)
	// {
	// 	FTimerHandle TimerHandle;
	// 	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ULanternWidget::HideText, 2.0f, false);
	// }
}

void ULanternWidget::HideText()
{
	// TB_Text->SetVisibility(ESlateVisibility::Hidden);
	// UE_LOG(LogTemp, Warning, TEXT("LanternWidget::HideWidget"));
}

