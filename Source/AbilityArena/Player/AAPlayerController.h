// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/AAAbilityStat.h"
#include "GameFramework/PlayerController.h"
#include "AAPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API AAAPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	AAAPlayerController();
	//ver0.8.1b 
	//get WeaponData
	TObjectPtr<class UAAWeaponItemData> SetInitData();
	//ver0.8.1b
	//Get PlayerStat
	FAAAbilityStat SendGameInstance();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;

	// ver 0.13.5a
	// Others Hp Bar Section
	UFUNCTION()
	void HandleSeamlessTravelComplete();

private:
	UPROPERTY()
	TObjectPtr<class UAAGameInstance> pc;

// ver 0.9.1a
// Level Change Event
public:
	UFUNCTION()
	void OnLevelChanged();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CreateUI();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void RemoveUI();

	// ver 0.10.1a
	// rename function. card select after create to user widget
	UFUNCTION(BlueprintCallable, Category = "UI")
	void RefreshUI();

	void BindSeamlessTravelEvent();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UUserWidget> PlayerUI;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void CreateCardSelectUI(TSubclassOf<UUserWidget> CardSelectUI);

	// ver 0.10.2a
	// Client Create UI & Random card pick
	UFUNCTION(Client, Reliable)
	void ClientRPCCreateCardSelectUI(TSubclassOf<UUserWidget> CardSelectUI);

	UFUNCTION()
	void SimulateRandomButtonClick();

	UFUNCTION(Client, Reliable)
	void ClientRPCSimulateRandomButtonClick();

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PlayerUIClass;

// ver 0.10.3a
// user card pick status check
protected:
	bool bIsPick;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetPickUpCard() { bIsPick = true; }

// ver 0.12.1a
// Score Section & Access Steam ID
	UFUNCTION(BlueprintCallable, Category = "Steam")
	FString GetSteamID() const;

	UFUNCTION(BlueprintCallable, Category = "Steam")
	FString GetSteamNickName() const;

	UFUNCTION(Client, Reliable)
	void ClientRPCCreateGameResultUI(const FString& InSteamNickName);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CreateGameResultUI(const FString& InSteamNickName);

	void AddScore(const FString& InSteamID, int32 InScore);

	UFUNCTION(BlueprintCallable)
	int32 GetScore(const FString& InSteamID) const;

	//TEST Input ID
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSteamID(const FString& InSteamID, const FString& InSteamNickName);

	UFUNCTION(Client, Reliable)
	void ClientSetSteamID(const FString& InSteamID, const FString& InSteamNickName);
private:
	FString SteamID;
	FString SteamNickName;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameResultUIClass;

	TObjectPtr<UUserWidget> GameResultUI;

	void SetSteamIDInPlayerState();

// ver 0.13.5a
// User Synchronize & Score UI
public:
	UFUNCTION(Client, Reliable)
	void ClientRPCAddScoreWidget(TSubclassOf<UUserWidget> WidgetClass, const TArray<FString>& PlayerNickNames, const TArray<int32>& PlayerScores);

	UFUNCTION(Client, Reliable)
	void ClientRPCRemoveScoreWidget();

private:
	UUserWidget* ScoreWidget;
};
