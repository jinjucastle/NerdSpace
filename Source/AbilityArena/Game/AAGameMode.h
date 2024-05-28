// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AAGameMode.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSeamlessTravelComplete);

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
	void FinishGame();
	FTimerHandle GameTimerHandle;

// ver 0.9.1a
public:
	virtual void PostSeamlessTravel() override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSeamlessTravelComplete OnSeamlessTravelComplete;
};
