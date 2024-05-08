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
	
	//ver 0.5.1b 
	//feat:check User UniqueID in Generate client using playerController
	//virtual void PostLogin(APlayerController* NewPlayer) override;

	

	void FinishGame();
	FTimerHandle GameTimerHandle;

};
