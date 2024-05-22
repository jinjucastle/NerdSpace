// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AAGameMode.h"
#include "Game/AAGameStateT.h"
#include "GameData/AAGameInstance.h"
#include "Player/AAPlayerController.h"
#include "Character/AACharacterPlayer.h"
#include "Item/AAWeaponItemData.h"
#include "CharacterStat/AACharacterPlayerState.h"
#include "Player/AAPlayerController.h"


AAAGameMode::AAAGameMode()
{
	//ver 0.5.1b
	//feat: playerStateID가 seamlessTravel에는 변경 X
	bUseSeamlessTravel = true;
}

void AAAGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//라운드 활동 부분 0.3.3B
	// 충돌가능성으로 인한 주석처리
	//GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AAAGameMode::DefaultGameTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void AAAGameMode::DefaultGameTimer()
{
	AAAGameStateT* const AAGameStateT = Cast<AAAGameStateT>(GameState);
	

	if (AAGameStateT && AAGameStateT->RemainingTime > 0)
	{
		AAGameStateT->RemainingTime--;
		//0.3.3b LogMessage
		UE_LOG(LogTemp, Warning, TEXT("RemainingTime: %d"),AAGameStateT->RemainingTime);
		if (AAGameStateT->RemainingTime <= 0)
		{
			if (GetMatchState() == MatchState::InProgress)
			{
				FinishGame();
				
			}
			else if(GetMatchState()==MatchState::WaitingPostMatch)
			{
				
				// 0.5.1b
				//feat: change function Servertravel->SeamlessTravel
				GetWorld()->SeamlessTravel(TEXT("/Game/Maps/Test"),true);
				
				
			}
		}
	}


}

//다른 곳과 충돌가능성이 있기 때문에 주석 처리
/*void AAAGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
		
	UAAGameInstance* GI = Cast<UAAGameInstance>(GetGameInstance());
	AAACharacterPlayerState* PlayerState = Cast<AAACharacterPlayerState>(NewPlayer->PlayerState);
	//ver 0.5.1b
	//feat: PlayerID를 클라이언트가 생성되면 받아옴
	if (GI)
	{
	
		FPlayerInfo NewInfo;		
		NewInfo.PlayerID = PlayerState->GetPlayerId();
		GI->AddPlayerInfo(NewInfo);
		UE_LOG(LogTemp, Warning, TEXT("Add Player"));
		UE_LOG(LogTemp, Error, TEXT("Add Player number:%d"),GI->GetPlayerInfoNum());
		for (int32 i = 0; i < GI->GetPlayerInfoNum(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("Add Player number:%d"), GI->PlayerInfos[i].PlayerID);
		}
	}
	
}*/

void AAAGameMode::FinishGame()
{
	AAAGameStateT* const AAGameStateT = Cast<AAAGameStateT>(GameState);
	if (AAGameStateT&&IsMatchInProgress())
	{
		EndMatch();
	
	}
	AAGameStateT->RemainingTime = AAGameStateT->ShowResultWaitingTime;
}
