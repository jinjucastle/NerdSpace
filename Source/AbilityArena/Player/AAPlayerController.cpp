// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AAPlayerController.h"
#include "Game/AAGameMode.h"
#include "GameData/AAGameInstance.h"
#include "GameData/AAAbilityStat.h"
#include "Character/AACharacterPlayer.h"
#include "CharacterStat/AACharacterPlayerState.h"
#include "Item/AAWeaponItemData.h"
#include "Item/AAWeaponAmmo.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerInput.h"
#include "UI/AACardSelectUI.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlinefriendsInterface.h"

AAAPlayerController::AAAPlayerController()
{

}

TObjectPtr<class UAAWeaponItemData> AAAPlayerController::SetInitData()
{
	//ver 0.8.1b
	//bring weaponData in GameInstance
	TObjectPtr<class UAAWeaponItemData> newPoint;
	

	if (IsLocalController())
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			newPoint = PC->GetsetWeaponItemData();
		}
	}
	else
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			newPoint = PC->GetsetWeaponItemData();
		}
	}
	return newPoint;
}

FAAAbilityStat AAAPlayerController::SendGameInstance()
{
	//ver0.8.1b
	//bring SavedStat in GameInstance
	FAAAbilityStat SavedStat;
	if (IsLocalController())
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			SavedStat = PC->GetPlayerStat();
		}
	}
	else
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			SavedStat = PC->GetPlayerStat();
		}
	}
	return SavedStat;
}

void AAAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BindSeamlessTravelEvent();

	SetSteamIDAndNickName();
	
	UE_LOG(LogTemp, Log, TEXT("Controller Set Steam Id or NickName Complete."));

	if (AAAGameMode* GameMode = Cast<AAAGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->OnAllPlayersReady.AddDynamic(this, &AAAPlayerController::HandleSeamlessTravelComplete);
	}
}

void AAAPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	RemoveUI();

	if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
	{
		if (AAAGameMode* MyGameMode = Cast<AAAGameMode>(GameMode))
		{
			MyGameMode->OnSeamlessTravelComplete.RemoveDynamic(this, &AAAPlayerController::OnLevelChanged);
		}
	}
}

void AAAPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AAACharacterPlayer* NewPawn = Cast<AAACharacterPlayer>(InPawn);

	if (NewPawn && NewPawn->GetIsAlive())
	{
		if (AAAGameMode* GameMode = Cast<AAAGameMode>(GetWorld()->GetAuthGameMode()))
		{
			UE_LOG(LogTemp, Log, TEXT("Controller Possess Pawn Complete."));
			GameMode->PlayerPossessCompleted(this);
		}
	}
}

void AAAPlayerController::HandleSeamlessTravelComplete()
{
	if (IsLocalController())
	{
		UE_LOG(LogTemp, Log, TEXT("HandleSeamlessTravelComplete: UI initialized"));
	}
}

void AAAPlayerController::OnLevelChanged()
{
	UE_LOG(LogTemp, Error, TEXT("OnLevelChanged"));
	RemoveUI();
	CreateUI();
}

void AAAPlayerController::CreateUI()
{
	if (IsLocalController())
	{
		if (PlayerUIClass)
		{
			PlayerUI = CreateWidget<UUserWidget>(this, PlayerUIClass);
			if (PlayerUI)
			{
				PlayerUI->AddToViewport();
			}
		}
	}
}

void AAAPlayerController::RemoveUI()
{
	if (IsLocalController())
	{
		if (PlayerUI)
		{
			PlayerUI->RemoveFromParent();
			PlayerUI = nullptr;

			if (AAACharacterBase* PlayerCharacter = Cast<AAACharacterBase>(GetPawn())) PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		}
	}
}

void AAAPlayerController::RefreshUI()
{
	if (IsLocalController())
	{
		if (PlayerUI)
		{
			PlayerUI->RemoveFromParent();
			PlayerUI = nullptr;

			if (AAACharacterBase* PlayerCharacter = Cast<AAACharacterBase>(GetPawn())) PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

			// ver 0.10.1a
			// Only RefreshUI is client can moved character
			SetupGameInputMode();
		}
	}
}

void AAAPlayerController::BindSeamlessTravelEvent()
{
	if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
	{
		if (AAAGameMode* MyGameMode = Cast<AAAGameMode>(GameMode))
		{
			MyGameMode->OnSeamlessTravelComplete.AddDynamic(this, &AAAPlayerController::OnLevelChanged);
			UE_LOG(LogTemp, Error, TEXT("Bound to OnSeamlessTravelComplete"));
		}
	}
}

void AAAPlayerController::CreateCardSelectUI(TSubclassOf<UUserWidget> CardSelectUI)
{
	if (CardSelectUI)
	{
		RemoveUI();

		if (PlayerInput)
		{
			PlayerInput->FlushPressedKeys();
			UE_LOG(LogTemp, Log, TEXT("FlushPressedKeys called."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerInput is nullptr. Cannot flush pressed keys."));
		}

		SetupUIInputmode();

		PlayerUI = CreateWidget<UUserWidget>(this, CardSelectUI);
		if (PlayerUI)
		{
			PlayerUI->AddToViewport();

			if (AAACharacterPlayer* PlayerCharacter = Cast<AAACharacterPlayer>(GetPawn()))
			{
				PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

				if (PlayerCharacter->GetCurrentCharacterZoomType() == ECharacterZoomType::ZoomIn)
				{
					PlayerCharacter->ChangeZoom();
				}
			}

			bIsPick = false;
		}
	}
}

void AAAPlayerController::ClientRPCCreateCardSelectUI_Implementation(TSubclassOf<UUserWidget> CardSelectUI)
{
	FTimerHandle CreateUITimerHandle;

	SetIgnoreMoveInput(true);

	GetWorld()->GetTimerManager().SetTimer(
		CreateUITimerHandle,
		FTimerDelegate::CreateLambda([&]() {
			GetWorld()->GetTimerManager().ClearTimer(CreateUITimerHandle);
			}), 0.1f, false);

	CreateCardSelectUI(CardSelectUI);
}

void AAAPlayerController::SimulateRandomButtonClick()
{
	//After version rule for add a card
	//Button Widget Name is only "Button"
	if (PlayerUI && !bIsPick)
	{
		UAACardSelectUI* CardSelectUI = Cast<UAACardSelectUI>(PlayerUI->GetWidgetFromName(TEXT("WBP_CardSelectUI")));
		if (CardSelectUI)
		{
			if (UUserWidget* RandomWidget = CardSelectUI->GetRandomWidget())
			{
				if (UButton* Button = Cast<UButton>(RandomWidget->GetWidgetFromName(TEXT("Button"))))
				{
					Button->OnClicked.Broadcast();
					UE_LOG(LogTemp, Error, TEXT("Success Random Card Pick"));
				}
			}
		}
	}
}

void AAAPlayerController::ClientRPCCreateGameResultUI_Implementation(const FString& InSteamNickName)
{
	CreateGameResultUI(InSteamNickName);
}

void AAAPlayerController::ClientRPCSimulateRandomButtonClick_Implementation()
{
	SimulateRandomButtonClick();
}

void AAAPlayerController::ServerRPCSetPickUpCard_Implementation()
{
	bIsPick = true;

	if (AAAGameMode* GameMode = Cast<AAAGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->PlayerSelectCompleted();
	}
}

void AAAPlayerController::SetPickUpCard()
{
	if (HasAuthority())
	{
		bIsPick = true;

		if (AAAGameMode* GameMode = Cast<AAAGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->PlayerSelectCompleted();
		}
	}
	else
	{
		ServerRPCSetPickUpCard();
	}
}

FString AAAPlayerController::GetSteamID() const
{
	return SteamID;
}

FString AAAPlayerController::GetSteamNickName() const
{
	return SteamNickName;
}

void AAAPlayerController::CreateGameResultUI(const FString& InSteamNickName)
{
	if (GameResultUIClass)
	{
		RemoveUI();

		GameResultUI = CreateWidget<UUserWidget>(this, GameResultUIClass);
		if (GameResultUI)
		{
			GameResultUI->AddToViewport();

			UE_LOG(LogTemp, Log, TEXT("Creating Game Result UI for Winner: %s"), *InSteamNickName);

			if (GameResultUI->IsA<UUserWidget>())
			{
				UFunction* Func = GameResultUI->FindFunction(FName("SetWinnerName"));
				if (Func)
				{
					struct FWinnerName
					{
						FString WinnerNameString;
					};
					FWinnerName Params;
					Params.WinnerNameString = InSteamNickName;
					GameResultUI->ProcessEvent(Func, &Params);

					UE_LOG(LogTemp, Log, TEXT("SetWinnerName function called with name: %s"), *InSteamNickName);
				}
			}
		}
	}
}

void AAAPlayerController::AddScore(const FString& InSteamID, int32 InScore)
{
	UAAGameInstance* GameInstance = Cast<UAAGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->AddScore(InSteamID, InScore);
		UE_LOG(LogTemp, Warning, TEXT("Player %s new score: %d"), *InSteamID, GameInstance->GetScore(InSteamID));
	}
}

int32 AAAPlayerController::GetScore(const FString& InSteamID) const
{
	UAAGameInstance* GameInstance = Cast<UAAGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		return GameInstance->GetScore(InSteamID);
	}

	return 0;
}

//TEST Input ID
void AAAPlayerController::ClientSetSteamID_Implementation(const FString& InSteamID, const FString& InSteamNickName)
{
	if (IsLocalController())
	{
		SteamID = InSteamID;
		SteamNickName = InSteamNickName;
		SetSteamIDInPlayerState(SteamID, SteamNickName);
		UE_LOG(LogTemp, Log, TEXT("%s(%s) is ClientRPC SetSteamID"), *SteamID, *SteamNickName);
	}
}

bool AAAPlayerController::ServerSetSteamID_Validate(const FString& InSteamID, const FString& InSteamNickName)
{
	/*UAAGameInstance* MyGameInstace = Cast<UAAGameInstance>(GetGameInstance());
	return MyGameInstace->CheckScore(InSteamID);*/
	return true;
}

void AAAPlayerController::ServerSetSteamID_Implementation(const FString& InSteamID, const FString& InSteamNickName)
{
	SteamID = InSteamID;
	SteamNickName = InSteamNickName;
	ClientSetSteamID(SteamID, SteamNickName);
	SetSteamIDInPlayerState(SteamID, SteamNickName);

	UE_LOG(LogTemp, Log, TEXT("%s(%s) is ServerRPC SetSteamID"), *InSteamID, *InSteamNickName);
}

void AAAPlayerController::SetSteamIDInPlayerState(const FString& InSteamID, const FString& InSteamNickName)
{
	UAAGameInstance* GI = Cast<UAAGameInstance>(GetGameInstance());
	if (GI)
	{
		FString ID, NickName;
		GI->GetSteamData(ID, NickName);

		if (ID.IsEmpty() && NickName.IsEmpty())
		{
			GI->SetSteamData(InSteamID, InSteamNickName);

			if (AAACharacterPlayerState* PS = GetPlayerState<AAACharacterPlayerState>())
			{
				PS->SteamID = InSteamID;
				PS->SteamNickName = InSteamNickName;

				UE_LOG(LogTemp, Log, TEXT("%s(%s) is Set Player State"), *InSteamID, *InSteamNickName);
			}

			GI->GetSteamData(ID, NickName);
			UE_LOG(LogTemp, Log, TEXT("GameInstance Set NickName & ID: %s(%s) in SetSteamIDInPlayerState"), *NickName, *ID);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Already set NickName & ID: %s(%s) in SetSteamIDInPlayerState"), *NickName, *ID);

			if (AAACharacterPlayerState* PS = GetPlayerState<AAACharacterPlayerState>())
			{
				PS->SteamID = InSteamID;
				PS->SteamNickName = InSteamNickName;

				UE_LOG(LogTemp, Log, TEXT("%s(%s) is Set Player State"), *InSteamID, *InSteamNickName);
			}
		}
	}
}

void AAAPlayerController::SetSteamIDAndNickName()
{
	if (IsLocalController())
	{
		IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
		if (OnlineSubsystem)
		{
			IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
			if (Identity.IsValid())
			{
				FUniqueNetIdRepl UserId = Identity->GetUniquePlayerId(GetLocalPlayer()->GetControllerId());
				if (UserId.IsValid())
				{
					FString NewID = UserId.ToString();
					FString NewNickName = Identity->GetPlayerNickname(*UserId);
					SteamID = NewID;
					SteamNickName = NewNickName;
					if (HasAuthority())
					{
						SetSteamIDInPlayerState(SteamID, SteamNickName);

						UE_LOG(LogTemp, Log, TEXT("Set Steam ID and Nickname Complete Server: %s(%s)"), *SteamNickName, *SteamID);
					}
					else
					{
						ServerSetSteamID(NewID, NewNickName);

						UE_LOG(LogTemp, Log, TEXT("Set Steam ID and Nickname Complete Client: %s(%s)"), *SteamNickName, *SteamID);
					}
				}
			}
		}
	}
}

void AAAPlayerController::GetUserDataInGameInstance()
{
	UAAGameInstance* GI = Cast<UAAGameInstance>(GetGameInstance());
	if (GI)
	{
		FString ID, NickName;
		GI->GetSteamData(ID, NickName);

		if (HasAuthority())
		{
			SetSteamIDInPlayerState(ID, NickName);
		}
		else
		{
			ServerSetSteamID(ID, NickName);
		}

		UE_LOG(LogTemp, Log, TEXT("Set SteamData in PostSeamlessTravel: %s(%s)"), *NickName, *ID);
	}
}

void AAAPlayerController::SetupGameInputMode()
{
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
	CurrentInputMode = EControllerInputMode::GameOnly;
}

void AAAPlayerController::SetupUIInputmode()
{
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = true;
	CurrentInputMode = EControllerInputMode::UIOnly;

	if (PlayerInput)
	{
		PlayerInput->FlushPressedKeys();
	}
}

void AAAPlayerController::SetupDefrost()
{
	switch (CurrentInputMode)
	{
	case EControllerInputMode::UIOnly:
		SetupUIInputmode();
		break;
	case EControllerInputMode::GameOnly:
		SetupGameInputMode();
		break;
	}

	if (PlayerInput)
	{
		PlayerInput->FlushPressedKeys();
	}
}

void AAAPlayerController::SetupFreeze()
{
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);

	if (PlayerInput)
	{
		PlayerInput->FlushPressedKeys();
	}
}

void AAAPlayerController::OnPlayerDeath()
{
	LastPlayerPawn = GetPawn();
}

void AAAPlayerController::PossessLastPlayerPawn()
{
	if (LastPlayerPawn != nullptr)
	{
		UnPossess();
		LastPlayerPawn->SetActorEnableCollision(true);
		LastPlayerPawn->SetActorHiddenInGame(false);
		Possess(LastPlayerPawn);

		LastPlayerPawn = nullptr;
	}
}

void AAAPlayerController::ResetPlayerStat()
{
	if (HasAuthority())
	{
		if (IsLocalController())
		{
			UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
			if (PC)
			{
				if (DefaultWeaponData && DefaultAmmoClass)
				{
					PC->SetWeaponItemData(DefaultWeaponData);
					PC->SetAmmoClass(DefaultAmmoClass);
				}

				PC->SetPlayerStat(FAAAbilityStat());

				if (GameResultUI)
				{
					GameResultUI->RemoveFromParent();
				}
				RemoveUI();
			}
		}
		else
		{
			ClientRPCResetPlayerStat();
		}
	}
}

void AAAPlayerController::ClientRPCResetPlayerStat_Implementation()
{
	if (IsLocalController())
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			if (DefaultWeaponData && DefaultAmmoClass)
			{
				PC->SetWeaponItemData(DefaultWeaponData);
				PC->SetAmmoClass(DefaultAmmoClass);
			}

			PC->SetPlayerStat(FAAAbilityStat());

			if (GameResultUI)
			{
				GameResultUI->RemoveFromParent();
			}
			RemoveUI();
		}
	}
}

void AAAPlayerController::ClientRPCAddScoreWidget_Implementation(TSubclassOf<UUserWidget> WidgetClass, const TArray<FString>& PlayerNickNames, const TArray<int32>& PlayerScores)
{
	if (ScoreWidget != nullptr)
	{
		ScoreWidget->RemoveFromParent();
		ScoreWidget = nullptr;
	}

	if (WidgetClass != nullptr)
	{
		ScoreWidget = CreateWidget<UUserWidget>(this, WidgetClass);
		if (ScoreWidget != nullptr)
		{
			if (PlayerNickNames.Num() > 1)
			{
				for (int32 i = 0; i < PlayerNickNames.Num(); ++i)
				{
					FString PlayerNickName = PlayerNickNames[i];
					int32 PlayerScore = PlayerScores.IsValidIndex(i) ? PlayerScores[i] : 0;
					UFunction* Func = ScoreWidget->FindFunction(FName("AddPlayerScore"));
					if (Func)
					{
						struct FPlayerScore
						{
							FString NickName;
							int32 Score;
						};
						FPlayerScore Params;
						Params.NickName = PlayerNickName;
						Params.Score = PlayerScore;
						ScoreWidget->ProcessEvent(Func, &Params);
					}
				}
			}
			else
			{
				UFunction* Func = ScoreWidget->FindFunction(FName("FirstRoundStart"));
				if (Func)
				{
					struct FBlank
					{
					};
					FBlank Params;
					ScoreWidget->ProcessEvent(Func, &Params);
				}
			}

			ScoreWidget->AddToViewport();

			SetupUIInputmode();
		}
	}

	GetUserDataInGameInstance();
}

void AAAPlayerController::ClientRPCRemoveScoreWidget_Implementation()
{
	if (ScoreWidget != nullptr)
	{
		ScoreWidget->RemoveFromParent();
		ScoreWidget = nullptr;
	}

	RemoveUI();
	CreateUI();

	SetupGameInputMode();
}

