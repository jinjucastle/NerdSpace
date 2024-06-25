// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameData/AACharacterStat.h"
#include "AACharacterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API AAACharacterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AAACharacterPlayerState();
	
	void SetPresentWeaponData(TObjectPtr<class UAAWeaponItemData>& Weapondata);

	TObjectPtr<class UAAWeaponItemData> WeaponData;
	TObjectPtr<class UAAWeaponItemData> GetWeaponDat() { return PresentWeapon; }

private:
	UPROPERTY( VisibleAnywhere, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAAWeaponItemData> PresentWeapon;

// ver 0.12.2a
// Set ID
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	FString GetSteamID() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetSteamID(const FString& InSteamID);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	FString GetSteamNickName() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetSteamNickName(const FString& InSteamNickName);

private:
	UPROPERTY(Replicated)
	FString SteamID;

	UPROPERTY(Replicated)
	FString SteamNickName;
};
