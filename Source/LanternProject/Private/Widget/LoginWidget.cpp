// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LoginWidget.h"


void ULoginWidget::ShowLoginErrorMessage()
{
	SB_Toast->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
