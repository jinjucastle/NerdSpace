// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/AAItemData.h"
#include "GameData/AACharacterStat.h"
#include "AAWeaponItemData.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol = 0,
	Shotgun,
	Rifle,
	SniperRifle,
	Panzerfaust,
	Funny
};
/**
 * 
 */
UCLASS()
class NERDSPACE_API UAAWeaponItemData : public UAAItemData
{
	GENERATED_BODY()
	
 //ver0.0.1a
 //Log print
public:
	UAAWeaponItemData();

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("AAItemData", GetFName());
	}


 //ver0.0.1a
 //Insert Weapon Mesh & Stat
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = Bullet)
	TSoftObjectPtr<UStaticMesh> ShellMesh;

	UPROPERTY(EditAnywhere, Category = Bullet)
	TSoftObjectPtr<UStaticMesh> MagMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat)
	FAACharacterStat WeaponStat;

	// ver 0.0.2a
	// Add Ammo
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat)
	float AmmoDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat)
	float AmmoSpeed;

	UPROPERTY(EditAnywhere, Category = Stat)
	float AmmoPoolExpandSize;

	// ver 0.1.3a
	// Weapon Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EWeaponType Type;

public:

};
