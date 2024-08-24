// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AASesstionListUI.h"
#include "OnlineSubsystem.h"
#include "Online.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemSessionSettings.h"


void UAASesstionListUI::GetSessionInfo(FString SessionName)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	FName TestSession = FName(TEXT("GameSession"));
	UE_LOG(LogTemp, Log, TEXT("Player Name: %s"), *SessionName);
	
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		UE_LOG(LogTemp, Log, TEXT("Player Name: 1"));
		if (Sessions.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Player Name: 2"));

			FNamedOnlineSession* SessionDetails = Sessions->GetNamedSession(TestSession);		
			if (SessionDetails)
			{
				UE_LOG(LogTemp, Log, TEXT("Player Name: 3") );
				
				const TArray<TSharedRef<const FUniqueNetId>>& Players = SessionDetails->RegisteredPlayers;
				IOnlineIdentityPtr Identify = OnlineSub->GetIdentityInterface();
				
				for (const TSharedRef<const FUniqueNetId>& PlayerId : Players)
				{
					FString PlayerName;
					if (Identify.IsValid())
					{
						PlayerName = Identify->GetPlayerNickname(*PlayerId);
						UE_LOG(LogTemp, Log, TEXT("Player Name: %s"), *PlayerName);
					}
				}
			}

		}
		
	}
}



