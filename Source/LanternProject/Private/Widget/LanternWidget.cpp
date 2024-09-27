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

		if (NewText.Len() > 16 && NewText.Len() < 32)					// Case 1: Text only, up to 32 char
		{
			WidgetSwitcher->SetActiveWidgetIndex(1);
			VerticalTextSet(NewText, TB_Text_3, TB_Text_4, TB_Text_5, TB_Text_6);
			UE_LOG(LogTemp, Warning, TEXT("LanternWidget::InitWidget - Text only, up to 32 char"));
		}
		else if (Text.IsEmpty() && Texture != nullptr)				// Case 2: Image only 
		{
			WidgetSwitcher->SetActiveWidgetIndex(0);
			Img_Image->SetBrushFromTexture(Texture);
			TB_Text_1->SetText(FText::GetEmpty());
			TB_Text_2->SetText(FText::GetEmpty());
			UE_LOG(LogTemp, Warning, TEXT("LanternWidget::InitWidget - Image only"));
		}
		else									// Case 3: Image + Text, up to 16 char
		{
			WidgetSwitcher->SetActiveWidgetIndex(0);
			Img_Image->SetBrushFromTexture(Texture);
			VerticalTextSet(NewText, TB_Text_1, TB_Text_2);
			TB_Text_3->SetText(FText::GetEmpty());
			TB_Text_4->SetText(FText::GetEmpty());
			UE_LOG(LogTemp, Warning, TEXT("LanternWidget::InitWidget - Image with up to 16 char"));
		}

		if (NewText.Len() > 32)
		{
			UE_LOG(LogTemp, Warning, TEXT("Text is longer than 32 characters. Truncating text."));
			NewText = NewText.Left(32); // Truncate text to 32 characters
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

void ULanternWidget::VerticalTextSet(const FString& Text, UTextBlock* TextBox1, UTextBlock* TextBox2, UTextBlock* TextBox3, UTextBlock* TextBox4)
{
	TextBox1->SetText(FText::FromString(Text.Left(8)));

	if (Text.Len() > 8 && TextBox2)
	{
		TextBox2->SetText(FText::FromString(Text.Mid(8, 8)));
	}
	else if (TextBox2)
	{
		TextBox2->SetText(FText::GetEmpty());
	}

	if (Text.Len() > 16 && TextBox3)
	{
		TextBox3->SetText(FText::FromString(Text.Mid(16, 8)));
	}
	else if (TextBox3)
	{
		TextBox3->SetText(FText::GetEmpty());
	}

	if (Text.Len() > 24 && TextBox3)
	{
		TextBox4->SetText(FText::FromString(Text.Mid(24, 8)));
	}
	else if (TextBox4)
	{
		TextBox4->SetText(FText::GetEmpty());
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

