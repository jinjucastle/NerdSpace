// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/AACharacterStatComponent.h"
#include "GameData/AAGameSingleton.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UAACharacterStatComponent::UAACharacterStatComponent()
{
	CurrentLevel = 1;

	bWantsInitializeComponent = true;
}

void UAACharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	ResetStat();
	check(BaseStat.MaxHp > 0.0f);

	SetIsReplicated(true);
}

void UAACharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAACharacterStatComponent, CurrentHp);
	DOREPLIFETIME(UAACharacterStatComponent, MaxHp);
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

void UAACharacterStatComponent::HealHp(float InHealAmount)
{
	CurrentHp = FMath::Clamp(CurrentHp + InHealAmount, 0, MaxHp);
	OnHpChanged.Broadcast(CurrentHp, MaxHp);
}

float UAACharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UAACharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);

	OnHpChanged.Broadcast(CurrentHp, MaxHp);
}

void UAACharacterStatComponent::ResetStat()
{
	SetBaseStat(UAAGameSingleton::Get().GetCharacterStat(CurrentLevel));
	MaxHp = BaseStat.MaxHp;
	SetHp(MaxHp);
}

void UAACharacterStatComponent::OnRep_MaxHp()
{
	OnHpChanged.Broadcast(CurrentHp, MaxHp);
}

void UAACharacterStatComponent::OnRep_CurrentHp()
{
	OnHpChanged.Broadcast(CurrentHp, MaxHp);
	if (CurrentHp <= 0.0f)
	{
		OnHpZero.Broadcast();
	}
}

void UAACharacterStatComponent::SetNewMaxHp(const float NewMaxHp)
{
	float PrevMaxHp = MaxHp;
	MaxHp = NewMaxHp;
	if (PrevMaxHp != MaxHp)
	{
		SetHp(MaxHp);
		OnHpChanged.Broadcast(CurrentHp, MaxHp);
	}
}
