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
	

	

private:

	UPROPERTY()
	class UAAGameInstance* pc;


};
