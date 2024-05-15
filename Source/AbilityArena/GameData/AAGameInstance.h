// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
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
	
public:
	//ver 0.4.2b
	// Set Save weaponData
	void SetWeaponItemData(TObjectPtr<class UAAWeaponItemData> SetPresentWeapon);
	//ver 0.4.2b
	//Get Weapondata
	TObjectPtr<class UAAWeaponItemData> GetsetWeaponItemData();

	void AddPlayerInfo(const FPlayerInfo& newPlayerInfo);
	int32 GetPlayerInfoNum() { return PlayerInfos.Num(); }
	
	UPROPERTY(Transient, VisibleAnywhere)
		TArray <FPlayerInfo>PlayerInfos;


	int16 testNum = 0;
};
