// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/AACharacterStat.h"
#include "Item/AAWeaponItemData.h"
#include "AAHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API UAAHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UAAHUDWidget(const FObjectInitializer& ObjectInitializer);

public:
	void UpdateStat(const FAACharacterStat& CharacterStat, const UAAWeaponItemData& WeaponStat);
	void UpdateHpBar(float NewCurrentHp);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<class UAAHpBarWidget> HpBar;

	UPROPERTY()
	TObjectPtr<class UAACharacterStatWidget> CharacterAndWeaponStat;

};
