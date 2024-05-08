// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/AACharacterStatComponent.h"
#include "GameData/AAGameSingleton.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UAACharacterStatComponent::UAACharacterStatComponent()
{
	CurrentLevel = 1;

	bWantsInitializeComponent = true;

	//ver 0.5.1C
	MaxHp = 200.0f;
	CurrentHp = MaxHp;
}

void UAACharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetBaseStat(UAAGameSingleton::Get().GetCharacterStat(CurrentLevel));
	check(BaseStat.MaxHp > 0.0f);

	SetIsReplicated(true);
}

void UAACharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UAACharacterStatComponent, BaseStat, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UAACharacterStatComponent, WeaponStat, COND_OwnerOnly);
}

void UAACharacterStatComponent::OnRep_BaseStat()
{
	UE_LOG(LogTemp, Warning, TEXT("Called OnRep_BaseStat"));
	OnStatChanged.Broadcast(BaseStat, WeaponStat);

}

void UAACharacterStatComponent::OnRep_WeaponStat()
{
	UE_LOG(LogTemp, Warning, TEXT("Called OnRep_WeaponStat"));
	OnStatChanged.Broadcast(BaseStat, WeaponStat);
}
