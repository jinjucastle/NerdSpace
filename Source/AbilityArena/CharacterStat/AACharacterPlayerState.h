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
	

	void SetWeaponState(TObjectPtr<class UAAWeaponItemData> SetPresentWeapon );
	
	
	TObjectPtr<class UAAWeaponItemData>GetWeapon() { return PresentWeapon; }
	
private:

	UPROPERTY(Transient,VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UAAWeaponItemData> PresentWeapon;
};
