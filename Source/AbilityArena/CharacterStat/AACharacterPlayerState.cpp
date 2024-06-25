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
	bReplicates = true;
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

void AAACharacterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAACharacterPlayerState, SteamID);
	DOREPLIFETIME(AAACharacterPlayerState, SteamNickName);
}

FString AAACharacterPlayerState::GetSteamID() const
{
	return SteamID;
}

void AAACharacterPlayerState::SetSteamID(const FString& InSteamID)
{
	SteamID = InSteamID;
}

FString AAACharacterPlayerState::GetSteamNickName() const
{
	return SteamNickName;
}

void AAACharacterPlayerState::SetSteamNickName(const FString& InSteamNickName)
{
	SteamNickName = InSteamNickName;
}
