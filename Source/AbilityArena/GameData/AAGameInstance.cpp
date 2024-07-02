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

void UAAGameInstance::AddScore(const FString& InSteamID, int32 InPoints)
{
	if (Score.Contains(InSteamID))
	{
		Score[InSteamID] += InPoints;
	}
	else
	{
		Score.Add(InSteamID, InPoints);
		UE_LOG(LogTemp, Log, TEXT("Success Add %s"), *InSteamID);
	}
}

int32 UAAGameInstance::GetScore(const FString& InSteamID) const
{
	if (Score.Contains(InSteamID))
	{
		return Score[InSteamID];
	}
	return 0;
}

bool UAAGameInstance::CheckScore(const FString& InSteamID) const
{
	return Score.Contains(InSteamID);
}

void UAAGameInstance::SetSteamData(const FString& InSteamID, const FString& InSteamNickName)
{
	SteamID = InSteamID;
	SteamNickName = InSteamNickName;
}

void UAAGameInstance::GetSteamData(FString& InSteamID, FString& InSteamNickName) const
{
	InSteamID = SteamID;
	InSteamNickName = SteamNickName;
}

void UAAGameInstance::AddPlayerNickname(const FString& InSteamID, const FString& InSteamNickname)
{
	if (!PlayerNicknames.Contains(InSteamID))
	{
		PlayerNicknames.Add(InSteamID, InSteamNickname);
		UE_LOG(LogTemp, Log, TEXT("Added nickname %s for %s"), *InSteamNickname, *InSteamID);
	}
}

FString UAAGameInstance::GetPlayerNickname(const FString& InSteamID) const
{
	if (PlayerNicknames.Contains(InSteamID))
	{
		return PlayerNicknames[InSteamID];
	}
	return FString();
}

void UAAGameInstance::CreateSession()
{
	
}
