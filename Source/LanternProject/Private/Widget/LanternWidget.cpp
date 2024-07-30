// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LanternWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "TimerManager.h"

void ULanternWidget::InitWidget(UTexture2D* Texture, const FString& Text)
{
	if (nullptr == Texture && Text.IsEmpty())
	{
		Randomize();
	}
	else
	{
		FString NewText = Text.Replace(TEXT("\\n"), TEXT("\n"));
		if (Texture == nullptr)
		{
			// Image + Text : Index 0

			WidgetSwitcher->SetActiveWidgetIndex(0);
			Img_Image->SetBrushFromTexture(Texture);
			TB_Text->SetText(FText::FromString(NewText.Left(25)));

			// HideWidget();
			// UE_LOG(LogTemp, Warning, TEXT("HideWidget"));
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

void ULanternWidget::HideWidget()
{
	// PlayAnimation(HideWidgetAnim);

	if (TB_Text)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ULanternWidget::HideText, 2.0f, false);
	}
}

void ULanternWidget::HideText()
{
	TB_Text->SetVisibility(ESlateVisibility::Hidden);
	UE_LOG(LogTemp, Warning, TEXT("LanternWidget::HideWidget"));
}

