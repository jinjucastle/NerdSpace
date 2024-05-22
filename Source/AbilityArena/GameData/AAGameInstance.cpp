// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/AAGameInstance.h"

UAAGameInstance::UAAGameInstance()
{

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
	//UE_LOG(LogTemp, Warning, TEXT("Temp"));
}

TObjectPtr<class UAAWeaponItemData> UAAGameInstance::GetsetWeaponItemData()
{
	return PresentWeapon;
}

void UAAGameInstance::AddPlayerInfo(const FPlayerInfo& newPlayerInfo)
{
	PlayerInfos.Add(newPlayerInfo);
}
