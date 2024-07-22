// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LanternWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void ULanternWidget::InitWidget(UTexture2D* Texture, const FString& Text)
{
	if (nullptr == Texture && Text.IsEmpty())
	{
		Randomize();
	}
	else
	{
		// Modify newline character
		// FString NewText = Text.Replace(TEXT("\\n"), TEXT("\n"));
		// if (Texture == nullptr)
		// {
		// 	// Only Text : Index 1
		//
		// 	WidgetSwitcher->SetActiveWidgetIndex(1);
		// 	TB_Text2->SetText(FText::FromString(NewText.Left(42)));
		// }
		// else if (Text.IsEmpty())
		// {
		// 	// Only Image : Index 2
		//
		// 	WidgetSwitcher->SetActiveWidgetIndex(2);
		// 	Img_Image2->SetBrushFromTexture(Texture);
		// }
		// else
		// {
		// 	// Image + Text : Index 0
		//
		// 	WidgetSwitcher->SetActiveWidgetIndex(0);
		// 	Img_Image->SetBrushFromTexture(Texture);
		// 	TB_Text->SetText(FText::FromString(NewText.Left(25)));
		// }

		FString NewText = Text.Replace(TEXT("\\n"), TEXT("\n"));
		if (Texture == nullptr)
		{
			// Image + Text : Index 0

			WidgetSwitcher->SetActiveWidgetIndex(0);
			Img_Image->SetBrushFromTexture(Texture);
			TB_Text->SetText(FText::FromString(NewText.Left(25)));
		}
	}
}
void ULanternWidget::Randomize(int32 Type)
{
	if (Type == -1)
	{
		Type = FMath::RandRange(0, 2);
	}

	switch(Type)
	{
		// Text + Image
	case 0:
		WidgetSwitcher->SetActiveWidgetIndex(0);
		break;
		
		// Text Only
	case 1:
		WidgetSwitcher->SetActiveWidgetIndex(1);
		break;

		// Image Only
	case 2:
		WidgetSwitcher->SetActiveWidgetIndex(2);
		break;
	}
}
