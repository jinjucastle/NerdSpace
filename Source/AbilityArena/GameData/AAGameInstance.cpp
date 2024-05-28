// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/AAGameInstance.h"
#include "Game/AAGameMode.h"

UAAGameInstance::UAAGameInstance()
{
	bSetPlayerStatStay = false;
}
//ver 0.4.2b
// incomplete GameInstance
void UAAGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("CallGameInStance"));

}

void UAAGameInstance::SetWeaponItemData(TObjectPtr<class UAAWeaponItemData> SetPresentWeapon)
{
	PresentWeapon = SetPresentWeapon;
	
}

TObjectPtr<class UAAWeaponItemData> UAAGameInstance::GetsetWeaponItemData()
{
	return PresentWeapon;
}

void UAAGameInstance::SetPlayerStat(const FAAAbilityStat& NewPlayerStat)
{
	PlayerStat = NewPlayerStat;
	
}

FAAAbilityStat UAAGameInstance::GetPlayerStat()
{
	return PlayerStat;
}

void UAAGameInstance::SetSavePlayerStat(bool newbool)
{
	bSetPlayerStatStay = newbool;
}

bool UAAGameInstance::GetSavePlayerStat()
{
	return bSetPlayerStatStay;
}