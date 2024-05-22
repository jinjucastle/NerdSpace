// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AAPlayerController.h"
#include "GameData/AAGameInstance.h"
#include "Character/AACharacterBase.h"

AAAPlayerController::AAAPlayerController()
{

}

void AAAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
	
	
}
/*
void AAAPlayerController::OnPossess(APawn* inPawn)
{
	Super::OnPossess(inPawn);
	

	 if (IsLocalController())
	 {	
		 UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		 if (PC)
		 {

			 UE_LOG(LogAACharacter, Log, TEXT("CLient:%s"), *PC->GetName());
			 AAACharacterBase* po = Cast<AAACharacterBase>(inPawn);
			 if (po)
			 {
				 if (PC->GetsetWeaponItemData() == nullptr)
				 {
					 UE_LOG(LogAACharacter, Log, TEXT("OPERPORTN"));
				 }
				 else
				 {
					 po->SetWeaponDataBegin(pc->GetsetWeaponItemData());
				 }
			 }
		 }
	 }
	else
	{
		 UAAGameInstance* PC = Cast<UAAGameInstance>(GetGameInstance());
		if (PC)
		{
			UE_LOG(LogAACharacter, Log, TEXT("Server:%s"), *PC->GetName());
		}

			
	}
	

}
*/
