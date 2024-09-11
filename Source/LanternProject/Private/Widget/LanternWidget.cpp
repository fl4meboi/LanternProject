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
		// GameInstance->LogToFile(LOGTEXT(TEXT("LanternWidget::InitWidget->Randomize")));
	}
	else
	{
		FString NewText = Text.Replace(TEXT("\\n"), TEXT("\n"));

		if (Texture != nullptr)			// Case 1: uploaded image, up to 16 char in 2 text boxes
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
		else		// Case 2: no image, up to 32 char in 4 text boxes
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

		//if (NewText.Len() > 17)
		//{
		//	// Text + Text : Index 1

		//	FString FirstPart = NewText.Left(8);
		//	FString RemainingPart = NewText.Mid(8);

		//	WidgetSwitcher->SetActiveWidgetIndex(1);
		//	TB_Text_1->SetText(FText::FromString(FirstPart));
		//	TB_Text_2->SetText(FText::FromString(RemainingPart));

		//	UE_LOG(LogTemp, Warning, TEXT("LanternWidget::InitWidget-Over_8_Char"));
		//}
		//else
		//{
		//	// Image + Text : Index 0

		//	FString FirstPart = NewText.Left(8);
		//	FString RemainingPart = NewText.Mid(8);

		//	WidgetSwitcher->SetActiveWidgetIndex(0);
		//	Img_Image->SetBrushFromTexture(Texture);
		//	TB_Text_1->SetText(FText::FromString(FirstPart));
		//	TB_Text_2->SetText(FText::FromString(RemainingPart));

		//	UE_LOG(LogTemp, Warning, TEXT("LanternWidget::InitWidget-Under_8_Char"));
		//}

		// if (Texture == nullptr)
		// {
		// }
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
		// Text + Image
	case 0:
		WidgetSwitcher->SetActiveWidgetIndex(0);
		break;
		
		// Text + Text
	case 1:
		WidgetSwitcher->SetActiveWidgetIndex(1);
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

