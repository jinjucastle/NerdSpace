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

//0.10.1b add MapArray
void AAAGameMode::AddLevelName()
{
	FString ContentsPath = FPaths::ProjectContentDir() + TEXT("/Maps");
	FPaths::NormalizeDirectoryName(ContentsPath);
	IFileManager& FileManaget = IFileManager::Get();
	FString Searchpatten = TEXT("/*.umap");
	FileManaget.FindFiles(LevelArrary, *(ContentsPath+Searchpatten), true, false);

	UE_LOG(LogTemp, Log, TEXT("portoq") );
	for (const FString& FileNames : LevelArrary)
	{
		UE_LOG(LogTemp, Log, TEXT("portoq: %s"), *FileNames);
	}
}
//0.10.1b find randommapURL
FString AAAGameMode::SetTravelLevel()
{
	if (LevelArrary.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Level is Empty"));
	}
	FString RandomLevel;
	do {


		int32 RandomIndex = FMath::RandRange(0, LevelArrary.Num() - 1);
		RandomLevel = LevelArrary[RandomIndex];
	} while (RandomLevel == TEXT("TestTransitionMap.umap")&& RandomLevel == TEXT("Lobby.umap"));
	
	

	FString TotalLevel = TEXT("/Game/Maps/") + FPaths::GetBaseFilename(*RandomLevel);
	UE_LOG(LogTemp, Error, TEXT("Level Name Is:%s"), *TotalLevel);
	return  TotalLevel;
}

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
	AddLevelName();
}

void AAAGameMode::DefaultGameTimer()
{
	AAAGameStateT* const AAGameStateT = Cast<AAAGameStateT>(GameState);
	
	if (AAGameStateT && AAGameStateT->RemainingTime > 0)
	{
		AAGameStateT->RemainingTime--;
		//0.3.3b LogMessage
		UE_LOG(LogTemp, Log, TEXT("RemainingTime: %d"),AAGameStateT->RemainingTime);

		// ver 0.10.2a
		// until doesn't card pick 2 seconds before the level change
		if (GetMatchState() == MatchState::WaitingPostMatch && AAGameStateT->RemainingTime == 2)
		{
			RandomCardPick();
		}

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
				FString ChangeMap = SetTravelLevel();
				//UE_LOG(LogTemp, Log, TEXT("TEXT: %s"), *ChangeMap);
				GetWorld()->ServerTravel(*ChangeMap, true);
			
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

	// ver 0.10.1a
	// All client can't fire & add card select ui
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AAAPlayerController* PlayerController = Cast<AAAPlayerController>(It->Get()))
		{
			check(GetWorld()->GetNetMode() != NM_Client);
			if (AAACharacterPlayer* PlayerCharacter = Cast<AAACharacterPlayer>(PlayerController->GetPawn()))
			{
				PlayerCharacter->SetPlayerStopFire();
			}
			PlayerController->ClientRPCCreateCardSelectUI(CardSelectUIClass);
		}
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

// ver 0.10.2a
// All Client Random CardPick
void AAAGameMode::RandomCardPick()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AAAPlayerController* PlayerController = Cast<AAAPlayerController>(It->Get()))
		{
			check(GetWorld()->GetNetMode() != NM_Client);
			PlayerController->ClientRPCSimulateRandomButtonClick();
		}
	}
}

