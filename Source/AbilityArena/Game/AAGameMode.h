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


private:
	//0.10.1b LevelArray
	TArray<FString>LevelArrary;
	void AddLevelName();
	FString SetTravelLevel();
	
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
	
// ver 0.10.1a
// View Card Select UI Round end
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> CardSelectUIClass;

	// ver 0.10.2a
	// RandomCardPick Section
	void RandomCardPick();

// ver 0.11.4a
// Spawn Point array
public:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;

private:
	void InitializeSpawnPoints();
	AActor* GetRandomAvailableSpawnPoint();

	TArray<AActor*> PlayerStartPoints;
	TSet<AActor*> UsedPlayerStartPoints;

// ver 0.11.6a
// Round Change Gimmik
public:
	void PlayerDied(AController* PlayerController);

private:
	int32 AlivePlayers;

	void CheckForRoundEnd();
	void StartNextRound();

// ver 0.12.1a
// Score Section
protected:
	APlayerController* GetLastPlayerController();

	bool isFinishGame = false;

public:
	void CreateWinnerUI(class AAAPlayerController* WinnerController);
};
