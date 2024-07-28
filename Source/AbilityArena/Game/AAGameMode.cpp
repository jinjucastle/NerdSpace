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
#include "EngineUtils.h"

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
				if (AAGameStateT->RemainingTime == 5)
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
					ClearAllTimersInLevel(GetWorld());

					FTimerHandle DelayTimerHandle;

					GetWorld()->GetTimerManager().SetTimer(
						DelayTimerHandle,
						FTimerDelegate::CreateLambda([&]() {
							GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
							FString ChangeMap = SetTravelLevel();
							GetWorld()->ServerTravel(*ChangeMap, true);
							}), 0.3f, false);
				}

				else
				{
					for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
					{
						if (AAAPlayerController* PlayerController = Cast<AAAPlayerController>(It->Get()))
						{
							PlayerController->ResetPlayerStat();

							AAACharacterPlayer* ControlledPawn = Cast<AAACharacterPlayer>(PlayerController->GetPawn());
							if (ControlledPawn)
							{
								ControlledPawn->RemoveAllUI();
							}
						}
					}

					UAAGameInstance* GameInstance = Cast<UAAGameInstance>(GetGameInstance());
					if (GameInstance)
					{
						GameInstance->ResetAllScore();
					}

					ClearAllTimersInLevel(GetWorld());

					FTimerHandle DelayTimerHandle;

					GetWorld()->GetTimerManager().SetTimer(
						DelayTimerHandle,
						FTimerDelegate::CreateLambda([&]() {
							GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
							FString LobbyMap = TEXT("/Game/Maps/Lobby");
							GetWorld()->ServerTravel(*LobbyMap, true);
							}), 0.3f, false);
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
				PlayerController->PossessLastPlayerPawn();
				PlayerController->SetIgnoreMoveInput(true);

				FTimerHandle DelayTimerHandle;

				GetWorld()->GetTimerManager().SetTimer(
					DelayTimerHandle,
					FTimerDelegate::CreateLambda([&]() {
						GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
						}), 0.1f, false);

				PlayerController->SetupUIInputmode();
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

	CheckAllPlayersPossessed();
}

void AAAGameMode::Logout(AController* NewPlayer)
{
	Super::Logout(NewPlayer);

	NumPlayersPossessed--;
	AlivePlayers--;
	//TotalPlayers--;
	/*
	if(TotalPlayers == 1)
	{
		NoOneOtherPlayers();
	}
	
	else
	{
		AAAPlayerController* LogoutPlayer = Cast<AAAPlayerCharacter>(NewPlayer)
		if(LogoutPlayer)
		{
			if (UserController && HasAuthority())
			{
				FString SteamID = UserController->GetSteamID();
				UAAGameInstance* GameInstance = Cast<UAAGameInstance>(GetGameInstance());
				if (GameInstance)
				{
					GameInstance->RemoveScore(SteamID);
				}
			}
		}
	}
	*/
}

void AAAGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitializeSpawnPoints();

	UE_LOG(LogTemp, Warning, TEXT("Spawn points initialized. Total points: %d"), PlayerStartPoints.Num());
}

// ver 0.11.4a
// Spawn Point Setting
void AAAGameMode::InitializeSpawnPoints()
{
	PlayerStartPoints.Empty();
	UsedPlayerStartPoints.Empty();
	AvailableSpawnPoints.Empty();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAASpawnPoint::StaticClass(), PlayerStartPoints);

	UE_LOG(LogTemp, Warning, TEXT("Found %d spawn points."), PlayerStartPoints.Num());
}

AActor* AAAGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* ChosenSpawnPoint = GetRandomAvailableSpawnPoint();
	if (ChosenSpawnPoint)
	{
		UsedPlayerStartPoints.Add(ChosenSpawnPoint);
		UE_LOG(LogTemp, Warning, TEXT("Chosen Spawn Point: %s"), *ChosenSpawnPoint->GetName());
		return ChosenSpawnPoint;
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

AActor* AAAGameMode::GetRandomAvailableSpawnPoint()
{
	for (AActor* SpawnPoint : PlayerStartPoints)
	{
		if (!UsedPlayerStartPoints.Contains(SpawnPoint) && !IsSpawnPointOccupied(SpawnPoint))
		{
			AvailableSpawnPoints.Add(SpawnPoint);
		}
	}

	if (AvailableSpawnPoints.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, AvailableSpawnPoints.Num() - 1);
		return AvailableSpawnPoints[RandomIndex];
	}

	return nullptr;
}

bool AAAGameMode::IsSpawnPointOccupied(AActor* InSpawnPoint)
{
	FVector SpawnLocation = InSpawnPoint->GetActorLocation();
	float Radius = 100.0f;
	TArray<FOverlapResult> Overlaps;

	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(Radius);

	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		SpawnLocation,
		FQuat::Identity,
		ECC_Pawn,
		CollisionShape
	);

	return bHasOverlap;
}

void AAAGameMode::PlayerDied(AController* PlayerController)
{
	AlivePlayers--;

	AAAGameStateT* GS = Cast<AAAGameStateT>(GameState);
	GS->SetAlivePlayer(AlivePlayers);

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
				if (GameInstance->GetScore(SteamID) >= 3)
				{
					isFinishGame = true;
					CreateWinnerUI(UserController);
					UE_LOG(LogTemp, Error, TEXT("Winner is %s!!!"), *SteamNickName);
				}
			}
		}
		FinishGame();
	}
}

void AAAGameMode::NoOneOtherPlayers()
{
	AAAPlayerController* UserController = Cast<AAAPlayerController>(GetLastPlayerController());
	if (UserController && HasAuthority())
	{
		FString SteamID = UserController->GetSteamID();
		FString SteamNickName = UserController->GetSteamNickName();
		UE_LOG(LogTemp, Warning, TEXT("Calling ClientRPCAddScore for %s(%s)"), *SteamNickName, *SteamID);
		UserController->AddScore(SteamID, 3);
		UAAGameInstance* GameInstance = Cast<UAAGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s(%s) Score: %d after ClientRPCAddScore"), *SteamNickName, *SteamID, GameInstance->GetScore(SteamID));
			if (GameInstance->GetScore(SteamID) >= 3)
			{
				isFinishGame = true;
				CreateWinnerUI(UserController);
				UE_LOG(LogTemp, Error, TEXT("Winner is %s!!!"), *SteamNickName);
			}
		}
	}
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
				LastManCharacter->SetInvincibility();
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

void AAAGameMode::CheckAllPlayersPossessed()
{
	if (AlivePlayers >= TotalPlayers)
	{
		OnAllPlayersReady.Broadcast();
		StartGame();
	}
}

void AAAGameMode::CheckAllPlayersSelected()
{
	if (NumPlayersSelected == NumPlayersPossessed)
	{
		AAAGameStateT* const AAGameStateT = Cast<AAAGameStateT>(GameState);
		if (AAGameStateT->RemainingTime >= 5)
		{
			AAGameStateT->RemainingTime = 4;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Card Select Finish Players : %d / %d"), NumPlayersSelected, NumPlayersPossessed);

}

void AAAGameMode::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("All players (%d) are ready. Starting the game..."), AlivePlayers);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AAAPlayerController* PlayerController = Cast<AAAPlayerController>(It->Get());
		if (PlayerController && HasAuthority())
		{
			FString SteamID = PlayerController->GetSteamID();
			FString SteamNickName = PlayerController->GetSteamNickName();
			UAAGameInstance* GameInstance = Cast<UAAGameInstance>(GetGameInstance());
			if (GameInstance)
			{
				if (SteamID.Len() > 0 || SteamNickName.Len() > 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("%s is in game now (%s)"), *SteamNickName, *SteamID);
					GameInstance->AddScore(SteamID, 0);
					GameInstance->AddPlayerNickname(SteamID, SteamNickName);
				}
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
	AlivePlayers++;
	CheckAllPlayersPossessed();
}

void AAAGameMode::PlayerSelectCompleted()
{
	NumPlayersSelected++;
	CheckAllPlayersSelected();
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

					if (PlayerIDs.Num() > 1)
					{
						for (const FString& PlayerID : PlayerIDs)
						{
							FString PlayerNickName = GameInstance->GetPlayerNickname(PlayerID);
							int32 PlayerScore = GameInstance->GetScore(PlayerID);

							PlayerNickNames.Add(PlayerNickName);
							PlayerScores.Add(PlayerScore);
						}
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

void AAAGameMode::ClearAllTimersInLevel(UWorld* World)
{
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor)
		{
			// 타이머 매니저를 가져와 타이머 클리어
			Actor->GetWorldTimerManager().ClearAllTimersForObject(Actor);
		}
	}
}

void AAAGameMode::MulticastRPCShowScoreUI_Implementation()
{
	ShowScoreUI();
}
