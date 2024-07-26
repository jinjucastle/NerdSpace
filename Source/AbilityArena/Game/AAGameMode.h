// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AAGameMode.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSeamlessTravelComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPlayersReady);

UCLASS()
class ABILITYARENA_API AAAGameMode : public AGameMode
{
	GENERATED_BODY()


private:
	//0.10.1b LevelArray
	
	void AddLevelName();
	FString SetTravelLevel();
	
public:
	AAAGameMode();
	UPROPERTY(EditDefaultsOnly, Category = "Level")
	TArray<FString>LevelArrary;

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
	virtual void Logout(AController* NewPlayer) override;

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

// ver 0.13.5a
// User Synchronize
private:
	int32 NumPlayersPossessed;
	int32 NumPlayersSelected;
	int32 TotalPlayers;

	void CheckAllPlayersPossessed();
	void CheckAllPlayersSelected();
	void StartGame();

public:
	void PlayerPossessCompleted(APlayerController* NewPlayer);
	void PlayerSelectCompleted();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAllPlayersReady OnAllPlayersReady;

// View Score UI
protected:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowScoreUI();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideScoreUI();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCShowScoreUI();

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> ScoreWidgetClass;

// ver 0.17.4a
// All Object's Clear Timer in Level
	void ClearAllTimersInLevel(UWorld* World);
};
