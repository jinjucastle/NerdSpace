// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/AACharacterPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameData/AAGameInstance.h"
#include "Character/AACharacterPlayer.h"
#include "Kismet/GameplayStatics.h"

AAACharacterPlayerState::AAACharacterPlayerState()
{
	GameInstance = Cast<UAAGameInstance>(UGameplayStatics::GetGameInstance(this));
	PresentWeapon = WeaponData;
}

void AAACharacterPlayerState::SetPresentWeaponData(TObjectPtr<class UAAWeaponItemData>& Weapondata)
{
	PresentWeapon = Weapondata;
}

void AAACharacterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AAACharacterPlayerState, PresentWeapon, COND_OwnerOnly);
}

void AAACharacterPlayerState::OnRep_SetWeapon()
{
	if (GameInstance)
	{
		GameInstance->SetWeaponItemData(PresentWeapon);
		UE_LOG(LogTemp, Error, TEXT("Loding Weapon Name:%s"),*GameInstance->GetsetWeaponItemData().GetName());
	}
}
