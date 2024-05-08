// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AAPlayerController.h"
#include "UI/AAHUDWidget.h"

AAAPlayerController::AAAPlayerController()
{
	static ConstructorHelpers::FClassFinder<UAAHUDWidget> AAHUDWidgetRef(TEXT("/Game/UI/WBP_AAHUD.WBP_AAHUD_C"));
	if (AAHUDWidgetRef.Class)
	{
		AAHUDWidgetClass = AAHUDWidgetRef.Class;
  }
}

void AAAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	AAHUDWidget = CreateWidget<UAAHUDWidget>(this, AAHUDWidgetClass);
	if (AAHUDWidget)
	{
		AAHUDWidget->AddToViewport();
	}
}
