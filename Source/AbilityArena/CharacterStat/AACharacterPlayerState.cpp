// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/AACharacterPlayerState.h"
#include "Character/AACharacterPlayer.h"


void AAACharacterPlayerState::SetWeaponState(TObjectPtr<class UAAWeaponItemData> SetPresentWeapon)
{
	UE_LOG(LogTemp, Warning, TEXT("CallStateStore"));
	PresentWeapon = SetPresentWeapon;
}





