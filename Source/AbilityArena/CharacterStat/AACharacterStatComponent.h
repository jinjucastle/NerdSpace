// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/AACharacterStat.h"
#include "Item/AAWeaponItemData.h"
#include "AACharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, const FAACharacterStat& /*BaseStat*/, const FAACharacterStat& /*WeaponStat*/);	// ver0.0.1a Stat Delegate


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABILITYARENA_API UAACharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAACharacterStatComponent();

	virtual void InitializeComponent() override;

 //ver0.0.1a
 //Stat Component Base Work
	FOnStatChangedDelegate OnStatChanged;

	FORCEINLINE const FAACharacterStat& GetBaseStat() const { return BaseStat; }
	FORCEINLINE const FAACharacterStat& GetWeaponStat() const { return WeaponStat; }
	FORCEINLINE FAACharacterStat GetTotalStat() const { return BaseStat + WeaponStat; }

	FORCEINLINE void AddBaseStat(const FAACharacterStat& InAddBaseStat) { BaseStat = BaseStat + InAddBaseStat; OnStatChanged.Broadcast(GetBaseStat(), GetWeaponStat()); }
	FORCEINLINE void SetBaseStat(const FAACharacterStat& InAddBaseStat) { BaseStat = InAddBaseStat; OnStatChanged.Broadcast(GetBaseStat(), GetWeaponStat()); }
	FORCEINLINE void SetWeaponStat(const FAACharacterStat& InAddWeaponStat) { WeaponStat = InAddWeaponStat; OnStatChanged.Broadcast(GetBaseStat(), GetWeaponStat()); }

//ver0.0.1a
//Stat Component Base Work
protected:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BaseStat, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	FAACharacterStat BaseStat;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_WeaponStat, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	FAACharacterStat WeaponStat;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float CurrentLevel;

// ver 0.1.1a
// Server Replicate
protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_BaseStat();

	UFUNCTION()
	void OnRep_WeaponStat();

//ver 0.5.1 C
public:
	FORCEINLINE float GetMaxHp() { return MaxHp; }
	FORCEINLINE float GetCurrentHp() { return CurrentHp; }

protected:
	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float MaxHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;
};
