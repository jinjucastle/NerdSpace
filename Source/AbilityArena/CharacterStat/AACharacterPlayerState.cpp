// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/AACharacterPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameData/AAGameInstance.h"
#include "Character/AACharacterPlayer.h"

#include "Kismet/GameplayStatics.h"

AAACharacterPlayerState::AAACharacterPlayerState()
{
	PresentWeapon = nullptr;
	
	
}

void AAACharacterPlayerState::SetPresentWeaponData(TObjectPtr<class UAAWeaponItemData>& Weapondata)
{
	
	
	GameInstance = Cast<UAAGameInstance>(GetGameInstance());
	

	PresentWeapon = Weapondata;
	GameInstance->SetWeaponItemData(PresentWeapon);
	//UE_LOG(LogTemp, Error, TEXT("Loding Weapon Name:%s"),*GameInstance->GetName());
	
}





