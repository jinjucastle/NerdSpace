// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AAGameMode.h"
#include "Game/AAGameStateT.h"
#include "GameData/AAGameInstance.h"
#include "Player/AAPlayerController.h"
#include "Character/AACharacterPlayer.h"
#include "Item/AAWeaponItemData.h"
#include "CharacterStat/AACharacterPlayerState.h"
#include "Player/AAPlayerController.h"
#include "Blueprint/UserWidget.h"


AAAGameMode::AAAGameMode()
{
	//ver 0.5.1b
	//feat: playerStateID가 seamlessTravel에는 변경 X
	bUseSeamlessTravel = true;
	
	static ConstructorHelpers::FClassFinder<AActor> ActorBPClass(TEXT("/Script/Engine.Blueprint'/Game/Blueprint/Item/BP_AAItemBox.BP_AAItemBox_C'"));
	if (ActorBPClass.Class != nullptr)
	{
		BlueprintActorClass = ActorBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> CardSelectWBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/TestUI_2.TestUI_2_C'"));
	if (CardSelectWBPClass.Class != nullptr)
	{
		CardSelectUIClass = CardSelectWBPClass.Class;
	}
}

void AAAGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//라운드 활동 부분 0.3.3B
	// 충돌가능성으로 인한 주석처리
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AAAGameMode::DefaultGameTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void AAAGameMode::DefaultGameTimer()
{
	AAAGameStateT* const AAGameStateT = Cast<AAAGameStateT>(GameState);
	
	if (AAGameStateT && AAGameStateT->RemainingTime > 0)
	{
		AAGameStateT->RemainingTime--;
		//0.3.3b LogMessage
		UE_LOG(LogTemp, Log, TEXT("RemainingTime: %d"),AAGameStateT->RemainingTime);
		if (AAGameStateT->RemainingTime <= 0)
		{
			if (GetMatchState() == MatchState::InProgress)
			{
				FinishGame();
			}
			else if(GetMatchState()==MatchState::WaitingPostMatch)
			{
				// 0.9.1b
				//feat: change function SeamlessTravel->Servertravel
				
				GetWorld()->ServerTravel(TEXT("/Game/Maps/Test_2"),true);
				
			}
		}
	}
}

void AAAGameMode::FinishGame()
{
	AAAGameStateT* const AAGameStateT = Cast<AAAGameStateT>(GameState);
	if (AAGameStateT&&IsMatchInProgress())
	{
		EndMatch();
	}
	AAGameStateT->RemainingTime = AAGameStateT->ShowResultWaitingTime;

	for (TPlayerControllerIterator<AAAPlayerController>::ServerAll It(GetWorld()); It; ++It)
	{
		AAAPlayerController* PlayerController = *It;

		check(GetWorld()->GetNetMode() != NM_Client);
		if (AAACharacterPlayer* PlayerCharacter = Cast<AAACharacterPlayer>(PlayerController->GetPawn()))
		{
			PlayerCharacter->SetPlayerStopFire();
		}

		PlayerController->CreateCardSelectUI(CardSelectUIClass);
	}
}

void AAAGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	UE_LOG(LogTemp, Error, TEXT("PostSeamlessTravel"));

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AAAPlayerController* MyPlayerController = Cast<AAAPlayerController>(It->Get()))
		{
			MyPlayerController->BindSeamlessTravelEvent();
		
		}
	}

	OnSeamlessTravelComplete.Broadcast();
	
}

