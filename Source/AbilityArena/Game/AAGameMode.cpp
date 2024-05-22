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
	//feat: playerStateID�� seamlessTravel���� ���� X
	bUseSeamlessTravel = true;
}

void AAAGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//���� Ȱ�� �κ� 0.3.3B
	// �浹���ɼ����� ���� �ּ�ó��
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

//�ٸ� ���� �浹���ɼ��� �ֱ� ������ �ּ� ó��
/*void AAAGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
		
	UAAGameInstance* GI = Cast<UAAGameInstance>(GetGameInstance());
	AAACharacterPlayerState* PlayerState = Cast<AAACharacterPlayerState>(NewPlayer->PlayerState);
	//ver 0.5.1b
	//feat: PlayerID�� Ŭ���̾�Ʈ�� �����Ǹ� �޾ƿ�
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
