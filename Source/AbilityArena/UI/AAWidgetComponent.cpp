// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AAWidgetComponent.h"
#include "AAUserWidget.h"

void UAAWidgetComponent::InitWidget()
{
	Super::InitWidget();

	UAAUserWidget* AAUserWidget = Cast<UAAUserWidget>(GetWidget());
	if (AAUserWidget)
	{
		AAUserWidget->SetOwningActor(GetOwner());
	}
}
