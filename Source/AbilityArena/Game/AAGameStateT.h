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

	int32 ShowResultWaitingTime = 15;

	FORCEINLINE void SetAlivePlayer(const int32 NewCount) { AlivePlayers = NewCount; }
	FORCEINLINE int32 GetAlivePlayer() const { return AlivePlayers; }

protected:
	UPROPERTY(Transient, Replicated)
	int32 AlivePlayers = 0;
};
