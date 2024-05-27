// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AAPlayerController.h"
#include "GameData/AAGameInstance.h"
#include "Character/AACharacterBase.h"
#include "Item/AAWeaponItemData.h"

AAAPlayerController::AAAPlayerController()
{

}

TObjectPtr<class UAAWeaponItemData> AAAPlayerController::SetInitData()
{
	//ver 0.8.1b
	//bring weaponData in GameInstance
	TObjectPtr<class UAAWeaponItemData> newPoint;
	

	if (IsLocalController())
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			newPoint = PC->GetsetWeaponItemData();
			/*UE_LOG(LogAACharacter, Log, TEXT("CLient:%s"), *PC->GetName());
			UE_LOG(LogAACharacter, Log, TEXT("CLient:%s"), *newPoint->GetName());
			*/
		}
		
		
	}
	else
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			newPoint = PC->GetsetWeaponItemData();

			/*UE_LOG(LogAACharacter, Log, TEXT("Server:%s"), *PC->GetName());
			UE_LOG(LogAACharacter, Log, TEXT("Server:%s"), *newPoint->GetName());*/
			
		}
	}
	return newPoint;
}

FAAAbilityStat AAAPlayerController::SendGameInstance()
{
	//ver0.8.1b
	//bring SavedStat in GameInstance
	FAAAbilityStat SavedStat;
	if (IsLocalController())
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			SavedStat = PC->GetPlayerStat();
			/*UE_LOG(LogAACharacter, Log, TEXT("LocalController:%s"), *PC->GetName());
			UE_LOG(LogAACharacter, Log, TEXT("LocalControllerDamage:%f"), SavedStat.Damage);*/
			
		}

	}
	else
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			SavedStat = PC->GetPlayerStat();
			/*UE_LOG(LogAACharacter, Log, TEXT("NotLocalController:%s"), *PC->GetName());
			UE_LOG(LogAACharacter, Log, TEXT("NotLocalControllerDamage:%f"), SavedStat.Damage);*/
		}
		
	}
	return SavedStat;
}

void AAAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
	
	
}


