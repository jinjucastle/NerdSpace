// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/AAGameInstance.h"

#include "Game/AAGameMode.h"

UAAGameInstance::UAAGameInstance()
{
	bSetPlayerStatStay = false;

	PlayerName = TEXT("Player Name");
}
//ver 0.4.2b
// incomplete GameInstance
void UAAGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("CallGameInStance"));
	
	Score.Empty();
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

void UAAGameInstance::AddScore(const FString& SteamID, int32 Points)
{
	if (Score.Contains(SteamID))
	{
		Score[SteamID] += Points;
	}
	else
	{
		Score.Add(SteamID, Points);
		UE_LOG(LogTemp, Log, TEXT("Success Add %s"), *SteamID);
	}
}

int32 UAAGameInstance::GetScore(const FString& SteamID) const
{
	if (Score.Contains(SteamID))
	{
		return Score[SteamID];
	}
	return 0;
}

bool UAAGameInstance::CheckScore(const FString& SteamID) const
{
	return Score.Contains(SteamID);
}

void UAAGameInstance::AddPlayerNickname(const FString& SteamID, const FString& Nickname)
{
	if (!PlayerNicknames.Contains(SteamID))
	{
		PlayerNicknames.Add(SteamID, Nickname);
		UE_LOG(LogTemp, Log, TEXT("Added nickname %s for %s"), *Nickname, *SteamID);
	}
}

FString UAAGameInstance::GetPlayerNickname(const FString& SteamID) const
{
	if (PlayerNicknames.Contains(SteamID))
	{
		return PlayerNicknames[SteamID];
	}
	return FString();
}

void UAAGameInstance::CreateSession()
{
	
}
