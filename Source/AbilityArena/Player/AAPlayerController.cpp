// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AAPlayerController.h"

AAAPlayerController::AAAPlayerController()
{

}

void AAAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
