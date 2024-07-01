// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AAGameMode.h"
#include "Game/AAGameStateT.h"
#include "GameData/AAGameInstance.h"
#include "Player/AAPlayerController.h"
#include "Player/AASpawnPoint.h"
#include "Character/AACharacterPlayer.h"
#include "Item/AAWeaponItemData.h"
#include "CharacterStat/AACharacterPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

//0.10.1b add MapArray
void AAAGameMode::AddLevelName()
{
	for (const FString& FileNames : LevelArrary)
	{
		UE_LOG(LogTemp, Log, TEXT("portoq: %s"), *FileNames);
	}
}
//0.10.1b find randommapURL
FString AAAGameMode::SetTravelLevel()
{
	FString RandomLevel;
	
	int32 RandomIndex = FMath::RandRange(0, LevelArrary.Num() - 1);
	RandomLevel = LevelArrary[RandomIndex];
	
	FString TotalLevel = TEXT("/Game/Maps/") + RandomLevel;
	//FString TotalLevel = TEXT("/Game/Maps/Test");

	UE_LOG(LogTemp, Error, TEXT("Level Name Is:%s"), *TotalLevel);
	return  TotalLevel;
}

AAAGameMode::AAAGameMode()
{
	//ver 0.5.1b
	//feat: playerStateID가 seamlessTravel에는 변경 X
	bUseSeamlessTravel = true;

	AlivePlayers = 0;

	NumPlayersLoggedIn = 0;
	TotalPlayers = 2;
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
	//FString ChangeMap = SetTravelLevel();
	if (AAGameStateT && AAGameStateT->RemainingTime > 0 && GetMatchState() != MatchState::InProgress)
	{
		if (GetMatchState() == MatchState::WaitingPostMatch)
		{

			AAGameStateT->RemainingTime--;
			if (!isFinishGame)
			{
				//0.3.3b LogMessage
				UE_LOG(LogTemp, Log, TEXT("Card Select RemainingTime: %d"), AAGameStateT->RemainingTime);
				// ver 0.10.2a
				// until doesn't card pick 2 seconds before the level change
				if (AAGameStateT->RemainingTime == 2)
				{
					RandomCardPick();
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Finish Game, Move To Lobby: %d"), AAGameStateT->RemainingTime);
			}
		}

		if (AAGameStateT->RemainingTime <= 0)
		{
			if(GetMatchState()==MatchState::WaitingPostMatch)
			{
				if (!isFinishGame)
				{
					// 0.9.1b
					//feat: change function SeamlessTravel->Servertravel
					FString ChangeMap = SetTravelLevel();
					//UE_LOG(LogTemp, Log, TEXT("TEXT: %s"), *ChangeMap);
					GetWorld()->ServerTravel(*ChangeMap, true);
				}
				else
				{
					FString LobbyMap = TEXT("/Game/Maps/Lobby");
					GetWorld()->ServerTravel(*LobbyMap, true);
				}
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
			if (AAACharacterPlayer* PlayerCharacter = Cast<AAACharacterPlayer>(PlayerController->GetPawn()))
			{
				PlayerCharacter->SetPlayerStopFire();
			}

			if (!isFinishGame)
			{
				PlayerController->ClientRPCCreateCardSelectUI(CardSelectUIClass);
			}
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

	PlayerStartPoints.Empty();
	UsedPlayerStartPoints.Empty();

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
			PlayerController->ClientRPCSimulateRandomButtonClick();
		}
	}
}


// ver 0.11.4a
// Spawn Point Setting
AActor* AAAGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* ChosenSpawnPoint = GetRandomAvailableSpawnPoint();
	if (ChosenSpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Chosen Spawn Point: %s"), *ChosenSpawnPoint->GetName());
		return ChosenSpawnPoint;
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void AAAGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (UsedPlayerStartPoints.Num() >= PlayerStartPoints.Num())
	{
		UsedPlayerStartPoints.Empty();
	}

	for (AActor* SpawnPoint : UsedPlayerStartPoints)
	{
		UE_LOG(LogTemp, Warning, TEXT("Used Spawn Point: %s"), *SpawnPoint->GetName());
	}

	AlivePlayers++;
	UE_LOG(LogTemp, Warning, TEXT("Found %d Alive Player"), AlivePlayers);

	NumPlayersLoggedIn++;

	CheckAllPlayersReady();
	CheckAllPlayersPossessed();
}

void AAAGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeSpawnPoints();

	UE_LOG(LogTemp, Warning, TEXT("Spawn points initialized. Total points: %d"), PlayerStartPoints.Num());
}

void AAAGameMode::InitializeSpawnPoints()
{
	PlayerStartPoints.Empty();
	UsedPlayerStartPoints.Empty();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAASpawnPoint::StaticClass(), PlayerStartPoints);

	UE_LOG(LogTemp, Warning, TEXT("Found %d spawn points."), PlayerStartPoints.Num());
}

AActor* AAAGameMode::GetRandomAvailableSpawnPoint()
{
	for (AActor* SpawnPoint : PlayerStartPoints)
	{
		if (!UsedPlayerStartPoints.Contains(SpawnPoint))
		{
			UsedPlayerStartPoints.Add(SpawnPoint);
			return SpawnPoint;
		}
	}

	return Super::ChoosePlayerStart_Implementation(nullptr);
}

void AAAGameMode::PlayerDied(AController* PlayerController)
{
	AlivePlayers--;

	// 플레이어가 죽을 때마다 라운드 종료 조건을 확인합니다.
	CheckForRoundEnd();

	UE_LOG(LogTemp, Warning, TEXT("Current %d Alive Player"), AlivePlayers);
}

void AAAGameMode::CheckForRoundEnd()
{
	if (AlivePlayers <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Last Man Standing"));

		AAAPlayerController* UserController = Cast<AAAPlayerController>(GetLastPlayerController());
		if (UserController && HasAuthority())
		{
			FString SteamID = UserController->GetSteamID();
			FString SteamNickName = UserController->GetSteamNickName();
			UE_LOG(LogTemp, Warning, TEXT("Calling ClientRPCAddScore for %s(%s)"), *SteamNickName, *SteamID);
			UserController->AddScore(SteamID, 1);
			UAAGameInstance* GameInstance = Cast<UAAGameInstance>(GetGameInstance());
			if (GameInstance)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s(%s) Score: %d after ClientRPCAddScore"), *SteamNickName, *SteamID, GameInstance->GetScore(SteamID));
				if (GameInstance->GetScore(SteamID) >= 5)
				{
					isFinishGame = true;
					CreateWinnerUI(UserController);
					UE_LOG(LogTemp, Error, TEXT("Winner is %s!!!"), *SteamNickName);
				}
			}
		}
		StartNextRound();
	}
}

void AAAGameMode::StartNextRound()
{
	UE_LOG(LogTemp, Warning, TEXT("Move to Next round"));
	FinishGame();
}

APlayerController* AAAGameMode::GetLastPlayerController()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AAAPlayerController* PC = Cast<AAAPlayerController>(It->Get());
		if (PC)
		{
			AAACharacterPlayer* LastManCharacter = Cast<AAACharacterPlayer>(PC->GetPawn());
			if (LastManCharacter && LastManCharacter->GetIsAlive())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s: %s is Alive"), *PC->GetSteamID(), *LastManCharacter->GetActorLocation().ToString());
				return PC;
			}
		}
	}
	return nullptr;
}

void AAAGameMode::CreateWinnerUI(AAAPlayerController* WinnerController)
{
	if (WinnerController)
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			AAAPlayerController* PlayerController = Cast<AAAPlayerController>(It->Get());
			if (PlayerController)
			{
				PlayerController->ClientRPCCreateGameResultUI(WinnerController->GetSteamNickName());
			}
		}
	}
}

void AAAGameMode::CheckAllPlayersReady()
{
	if (NumPlayersLoggedIn >= TotalPlayers)
	{
		OnAllPlayersReady.Broadcast();
		//StartGame();
	}
}

void AAAGameMode::CheckAllPlayersPossessed()
{
	if (NumPlayersPossessed >= TotalPlayers)
	{
		StartGame();
	}
}

void AAAGameMode::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("All players are ready. Starting the game..."));

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AAAPlayerController* PlayerController = Cast<AAAPlayerController>(It->Get());
		if (PlayerController && HasAuthority())
		{
			FString SteamID = PlayerController->GetSteamID();
			FString SteamNickName = Cast<AAACharacterPlayerState>(PlayerController->PlayerState)->GetSteamNickName();
			UAAGameInstance* GameInstance = Cast<UAAGameInstance>(GetGameInstance());
			if (GameInstance)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s is in game now (%s)"), *SteamNickName, *SteamID);
				PlayerController->AddScore(SteamID, 0);
				GameInstance->AddPlayerNickname(SteamID, SteamNickName);
			}
		}
	}

	ShowScoreUI();

	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AAAGameMode::HideScoreUI, 7.0f, false);
}

void AAAGameMode::PlayerPossessCompleted(APlayerController* NewPlayer)
{
	NumPlayersPossessed++;
	CheckAllPlayersPossessed();
}

void AAAGameMode::ShowScoreUI()
{
	if (ScoreWidgetClass != nullptr)
	{
		// Get all player controllers in the level
		TArray<AActor*> PlayerControllers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), PlayerControllers);

		for (AActor* PlayerControllerActor : PlayerControllers)
		{
			AAAPlayerController* PlayerController = Cast<AAAPlayerController>(PlayerControllerActor);
			if (PlayerController)
			{
				UAAGameInstance* GameInstance = Cast<UAAGameInstance>(GetGameInstance());
				if (GameInstance)
				{
					TArray<FString> PlayerIDs;
					TArray<FString> PlayerNickNames;
					TArray<int32> PlayerScores;

					GameInstance->Score.GenerateKeyArray(PlayerIDs);

					for (const FString& PlayerID : PlayerIDs)
					{
						FString PlayerNickName = GameInstance->GetPlayerNickname(PlayerID);
						int32 PlayerScore = GameInstance->GetScore(PlayerID);
						
						PlayerNickNames.Add(PlayerNickName);
						PlayerScores.Add(PlayerScore);
					}

					PlayerController->ClientRPCAddScoreWidget(ScoreWidgetClass, PlayerNickNames, PlayerScores);
				}
			}
		}
	}
}

void AAAGameMode::HideScoreUI()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AAAPlayerController* PlayerController = Cast<AAAPlayerController>(It->Get());
		if (PlayerController)
		{
			PlayerController->ClientRPCRemoveScoreWidget();
		}
	}
}

void AAAGameMode::MulticastRPCShowScoreUI_Implementation()
{
	ShowScoreUI();
}
