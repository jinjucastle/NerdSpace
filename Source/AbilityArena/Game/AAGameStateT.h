// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "AAGameStateT.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API AAAGameStateT : public AGameState
{
	GENERATED_BODY()

public:


	AAAGameStateT();

	virtual void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const;
	UPROPERTY(Transient, Replicated)
		int32 RemainingTime;
		
	int32 MatchPlayTime=60;

	int32 ShowResultWaitingTime = 5;

	
};
