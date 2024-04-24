// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AAGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API AAAGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AAAGameMode();
protected:

	virtual void PostInitializeComponents() override;

	virtual void DefaultGameTimer();

	void FinishGame();
	FTimerHandle GameTimerHandle;
};
