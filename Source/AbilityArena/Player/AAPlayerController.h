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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	UPROPERTY()
	class UAAGameInstance* pc;

// ver 0.9.1a
// Level Change Event
public:
	UFUNCTION()
	void OnLevelChanged();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CreateUI();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void RemoveUI();

	void BindSeamlessTravelEvent();

private:
	UPROPERTY()
	UUserWidget* PlayerUI;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PlayerUIClass;
};
