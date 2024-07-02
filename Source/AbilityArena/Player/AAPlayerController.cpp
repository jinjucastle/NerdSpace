// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AAPlayerController.h"
#include "Game/AAGameMode.h"
#include "GameData/AAGameInstance.h"
#include "Character/AACharacterPlayer.h"
#include "CharacterStat/AACharacterPlayerState.h"
#include "Item/AAWeaponItemData.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
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

	SetSteamIDAndNickName();

	if (AAAGameMode* GameMode = Cast<AAAGameMode>(GetWorld()->GetAuthGameMode()))
	{
		UE_LOG(LogTemp, Log, TEXT("Controller Possess Pawn Complete."));
		GameMode->PlayerPossessCompleted(this);
	}
}

void AAAPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	SetSteamIDAndNickName();
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
			SetInputMode(FInputModeGameOnly());
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

		PlayerUI = CreateWidget<UUserWidget>(this, CardSelectUI);
		if (PlayerUI)
		{
			PlayerUI->AddToViewport();
			SetShowMouseCursor(true);

			if (AAACharacterPlayer* PlayerCharacter = Cast<AAACharacterPlayer>(GetPawn()))
			{
				PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

				if (PlayerCharacter->GetCurrentCharacterZoomType() == ECharacterZoomType::ZoomIn)
				{
					PlayerCharacter->ChangeZoom();
				}
			}

			FInputModeUIOnly UIOnlyInputMode;
			SetInputMode(UIOnlyInputMode);

			bIsPick = false;
		}
	}
}

void AAAPlayerController::ClientRPCCreateCardSelectUI_Implementation(TSubclassOf<UUserWidget> CardSelectUI)
{
	CreateCardSelectUI(CardSelectUI);
}

void AAAPlayerController::SimulateRandomButtonClick()
{
	//After version rule for add a card
	//Button Widget Name is only "Button"
	if (PlayerUI && !bIsPick)
	{
		UAACardSelectUI* CardSelectUI = Cast<UAACardSelectUI>(PlayerUI->GetWidgetFromName(TEXT("WBP_CardSelectUI")));
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

void AAAPlayerController::ClientRPCCreateGameResultUI_Implementation(const FString& InSteamNickName)
{
	CreateGameResultUI(InSteamNickName);
}

void AAAPlayerController::ClientRPCSimulateRandomButtonClick_Implementation()
{
	SimulateRandomButtonClick();
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
	if (AAACharacterPlayer* CP = GetPawn<AAACharacterPlayer>())
	{
		if (AAACharacterPlayerState* PS = CP->GetPlayerState<AAACharacterPlayerState>())
		{
			PS->SetSteamID(InSteamID);
			PS->SetSteamNickName(InSteamNickName);

			UE_LOG(LogTemp, Log, TEXT("%s(%s) is Set Player State"), *InSteamID, *InSteamNickName);
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

						UE_LOG(LogTemp, Log, TEXT("OnPossess: Set Steam ID and Nickname Complete Server"));
					}
					else
					{
						ServerSetSteamID(NewID, NewNickName);

						UE_LOG(LogTemp, Log, TEXT("OnPossess: Set Steam ID and Nickname Complete Client"));
					}
				}
			}
		}
	}
}

void AAAPlayerController::ClientRPCAddScoreWidget_Implementation(TSubclassOf<UUserWidget> WidgetClass, const TArray<FString>& PlayerNickNames, const TArray<int32>& PlayerScores)
{
	if (ScoreWidget != nullptr)
	{
		ScoreWidget->RemoveFromViewport();
		ScoreWidget = nullptr;
	}

	if (WidgetClass != nullptr)
	{
		ScoreWidget = CreateWidget<UUserWidget>(this, WidgetClass);
		if (ScoreWidget != nullptr)
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

			ScoreWidget->AddToViewport();

			FInputModeUIOnly UIOnlyInputMode;
			SetInputMode(UIOnlyInputMode);
		}
	}
}

void AAAPlayerController::ClientRPCRemoveScoreWidget_Implementation()
{
	if (ScoreWidget != nullptr)
	{
		ScoreWidget->RemoveFromViewport();
		ScoreWidget = nullptr;
	}

	RefreshUI();
	CreateUI();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

