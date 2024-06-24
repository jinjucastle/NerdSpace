// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AAPlayerController.h"
#include "Game/AAGameMode.h"
#include "GameData/AAGameInstance.h"
#include "Character/AACharacterPlayer.h"
#include "CharacterStat/AACharacterPlayerState.h"
#include "Item/AAWeaponItemData.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/AACardSelectUI.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

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

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	BindSeamlessTravelEvent();

	CreateUI();

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
					if (HasAuthority())
					{
						FString NewID = UserId->ToString();
						SteamID = NewID;
						SetSteamIDInPlayerState();
					}
					else
					{
						FString NewID = UserId->ToString();
						ServerSetSteamID(NewID);
					}
				}
			}
		}
	}
	
	/*if (HasAuthority())
	{
		FString NewID = FString::FromInt(GetUniqueID());
		SteamID = NewID;
		SetSteamIDInPlayerState();
	}
	else
	{
		FString NewID = FString::FromInt(GetUniqueID());
		ServerSetSteamID(NewID);
	}*/
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

void AAAPlayerController::ClientRPCCreateGameResultUI_Implementation(const FString& InSteamID)
{
	CreateGameResultUI(InSteamID);
}

void AAAPlayerController::ClientRPCSimulateRandomButtonClick_Implementation()
{
	SimulateRandomButtonClick();
}

FString AAAPlayerController::GetSteamID() const
{
	return SteamID;
}

void AAAPlayerController::CreateGameResultUI(const FString& InSteamID)
{
	if (GameResultUIClass)
	{
		RemoveUI();

		GameResultUI = CreateWidget<UUserWidget>(this, GameResultUIClass);
		if (GameResultUI)
		{
			GameResultUI->AddToViewport();

			UE_LOG(LogTemp, Log, TEXT("Creating Game Result UI for Winner: %s"), *InSteamID);

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
					Params.WinnerNameString = InSteamID;
					GameResultUI->ProcessEvent(Func, &Params);

					UE_LOG(LogTemp, Log, TEXT("SetWinnerName function called with name: %s"), *InSteamID);
				}
			}
		}
	}
}

void AAAPlayerController::AddScore(const FString& InSteamID)
{
	if (HasAuthority())
	{
		UAAGameInstance* GameInstance = Cast<UAAGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			GameInstance->AddScore(SteamID, 1);
			UE_LOG(LogTemp, Warning, TEXT("Player %s new score: %d"), *SteamID, GameInstance->GetScore(SteamID));
		}
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
void AAAPlayerController::ClientSetSteamID_Implementation(const FString& InSteamID)
{
	if (IsLocalController())
	{
		SteamID = InSteamID;
	}
}

bool AAAPlayerController::ServerSetSteamID_Validate(const FString& NewSteamID)
{
	return true;
}

void AAAPlayerController::ServerSetSteamID_Implementation(const FString& NewSteamID)
{
	SteamID = NewSteamID;
	SetSteamIDInPlayerState();
	ClientSetSteamID(SteamID);
}

void AAAPlayerController::SetSteamIDInPlayerState()
{
	if (AAACharacterPlayerState* PS = GetPlayerState<AAACharacterPlayerState>())
	{
		PS->SetSteamID(SteamID);
	}
}

