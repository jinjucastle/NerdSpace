// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AAPlayerController.h"
#include "Game/AAGameMode.h"
#include "GameData/AAGameInstance.h"
#include "Character/AACharacterPlayer.h"
#include "Item/AAWeaponItemData.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/AACardSelectUI.h"

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
			/*UE_LOG(LogAACharacter, Log, TEXT("CLient:%s"), *PC->GetName());
			UE_LOG(LogAACharacter, Log, TEXT("CLient:%s"), *newPoint->GetName());
			*/
		}
		
		
	}
	else
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			newPoint = PC->GetsetWeaponItemData();

			/*UE_LOG(LogAACharacter, Log, TEXT("Server:%s"), *PC->GetName());
			UE_LOG(LogAACharacter, Log, TEXT("Server:%s"), *newPoint->GetName());*/
			
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
			/*UE_LOG(LogAACharacter, Log, TEXT("LocalController:%s"), *PC->GetName());
			UE_LOG(LogAACharacter, Log, TEXT("LocalControllerDamage:%f"), SavedStat.Damage);*/
			
		}

	}
	else
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			SavedStat = PC->GetPlayerStat();
			/*UE_LOG(LogAACharacter, Log, TEXT("NotLocalController:%s"), *PC->GetName());
			UE_LOG(LogAACharacter, Log, TEXT("NotLocalControllerDamage:%f"), SavedStat.Damage);*/
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
			UE_LOG(LogTemp, Error, TEXT("RemoveDynamic HandleSeamlessTravelComplete"));
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

void AAAPlayerController::ClientRPCSimulateRandomButtonClick_Implementation()
{
	SimulateRandomButtonClick();
}


