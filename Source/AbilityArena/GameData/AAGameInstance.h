// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "AAAbilityStat.h"

#include "AAGameInstance.generated.h"


/**
 * 
 */
//ver 0.5.1b
//feat: playerId ÀúÀå¿ë struct
USTRUCT(BlueprintType)
struct FPlayerInfo {

	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, Category = State)
		int32 PlayerID;
};

UCLASS()
class ABILITYARENA_API UAAGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UAAGameInstance();

	virtual void Init() override;
private:
	//ver 0.4.2b
	// initial stage of storing Waepondata
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UAAWeaponItemData> PresentWeapon;
	UPROPERTY()
	FAAAbilityStat PlayerStat;

	int8 bSetPlayerStatStay : 1;


public:
	//ver 0.4.2b
	// Set Save weaponData
	void SetWeaponItemData(TObjectPtr<class UAAWeaponItemData> SetPresentWeapon);
	//ver 0.4.2b
	//Get Weapondata
	TObjectPtr<class UAAWeaponItemData> GetsetWeaponItemData();
	//ver0.8.1b
	//Save playerStat
	void SetPlayerStat(const FAAAbilityStat& NewPlayerStat);
	//ver0.8.1b
	//Get PlayerStat
	FAAAbilityStat GetPlayerStat();

	void CreateSession();


	void SetSavePlayerStat(bool newBool);
	bool GetSavePlayerStat();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Ammo, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UClass> PlayerAmmoClass;

	FORCEINLINE void SetAmmoClass(const TObjectPtr<UClass> NewAmmoClass) { PlayerAmmoClass = NewAmmoClass; }
	FORCEINLINE UClass* GetAmmoClass() { return PlayerAmmoClass; }

	// ver 0.12.1a
	// Score Section
	void AddScore(const FString& SteamID, int32 Points);
	FORCEINLINE int32 GetScore(const FString& SteamID) const;

protected:
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, int32> Score;
};
