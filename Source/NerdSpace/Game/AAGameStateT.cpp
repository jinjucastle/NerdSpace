// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AAGameStateT.h"

#include "Net/UnrealNetwork.h"

AAAGameStateT::AAAGameStateT()
{
	//0.3.3b 라운드 시간 지정
	// 0.11.6a
	// Card Select Time
	RemainingTime = ShowResultWaitingTime;
}

void AAAGameStateT::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAAGameStateT, RemainingTime);
	DOREPLIFETIME(AAAGameStateT, AlivePlayers);
}