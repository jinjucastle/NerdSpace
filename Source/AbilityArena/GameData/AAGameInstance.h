// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AAGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API UAAGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	UAAGameInstance();

	virtual void Init() override;
private:
	//ver 0.4.2b
	// initial stage of storing Waepondata
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UAAWeaponItemData> PresentWeapon;
public:
	//ver 0.4.2b
	// Set Save weaponData
	void SetWeaponItemData(TObjectPtr<class UAAWeaponItemData> SetPresentWeapon);
	//ver 0.4.2b
	//Get Weapondata
	TObjectPtr<class UAAWeaponItemData> GetsetWeaponItemData();

};
