// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ErrorWidget.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"

bool UErrorWidget::Initialize()
{
	// return Super::Initialize();

	if (Super::Initialize() == false) return false;

	Brd_Toast->OnMouseButtonDownEvent.BindDynamic(this, &UErrorWidget::OnToastBorderMouseButtonDown);

	return true;
}

void UErrorWidget::ShowAlertMessage(const FString& Message)
{
	TB_Alert->SetText(FText::FromString(Message));
	Brd_Alert->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UErrorWidget::ShowToastMessage(const FString& Message)
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (HideToastTimerHandle.IsValid())
	{
		TimerManager.ClearTimer(HideToastTimerHandle);
	}

	TB_Toast->SetText(FText::FromString(Message));
	Brd_Toast->SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(ShowToastMessageAnimation);

	TimerManager.SetTimer(HideToastTimerHandle, this, &UErrorWidget::HideToastMessage, TimeToShowToastMessage, false);
}

void UErrorWidget::HideToastMessage()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (HideToastTimerHandle.IsValid())
	{
		TimerManager.ClearTimer(HideToastTimerHandle);
	}

	Brd_Toast->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimation(HideToastMessageAnimation);
}

FEventReply UErrorWidget::OnToastBorderMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		HideToastMessage();
		return FEventReply(true);
	}

	return FEventReply(false);
}
