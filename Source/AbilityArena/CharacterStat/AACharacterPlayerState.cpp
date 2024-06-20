// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/AACharacterPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameData/AAGameInstance.h"
#include "Character/AACharacterPlayer.h"
#include "Player/AAPlayerController.h"
#include "Kismet/GameplayStatics.h"

AAACharacterPlayerState::AAACharacterPlayerState()
{
	PresentWeapon = nullptr;
}

void AAACharacterPlayerState::SetPresentWeaponData(TObjectPtr<class UAAWeaponItemData>& Weapondata)
{
	AAAPlayerController* LocalPC = Cast<AAAPlayerController>(GetPlayerController());
	if (LocalPC)
	{
		UAAGameInstance* GameInstance = Cast<UAAGameInstance>(LocalPC->GetGameInstance());
		if (GameInstance)
		{
			PresentWeapon = Weapondata;

			//GameInstance->SetWeaponItemData(PresentWeapon);
			UE_LOG(LogTemp, Error, TEXT("Loding pontWeapon Name:%s"), *GameInstance->GetName());
		}
	}
}