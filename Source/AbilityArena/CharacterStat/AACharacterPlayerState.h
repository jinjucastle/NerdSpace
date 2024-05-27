// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameData/AACharacterStat.h"
#include "AACharacterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API AAACharacterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	
	AAACharacterPlayerState();

	
	void SetPresentWeaponData(TObjectPtr<class UAAWeaponItemData>& Weapondata);
	
	TObjectPtr<class UAAWeaponItemData> WeaponData;
	
	TObjectPtr<class UAAWeaponItemData> GetWeaponDat() { return PresentWeapon; }

	

private:

	UPROPERTY( VisibleAnywhere, Category = Stat, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UAAWeaponItemData> PresentWeapon;


protected:

	
	
};
