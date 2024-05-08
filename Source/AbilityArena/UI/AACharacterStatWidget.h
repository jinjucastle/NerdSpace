// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameData/AACharacterStat.h"
#include "Item/AAWeaponItemData.h"
#include "AACharacterStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API UAACharacterStatWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

public:
	void UpdateStat(const FAACharacterStat& CharacterStat, const UAAWeaponItemData& WeaponStat);

private:
	UPROPERTY()
	TMap<FName, class UTextBlock*> CharacterLookup;

	UPROPERTY()
	TMap<FName, class UTextBlock*> WeaponLookup;
};
