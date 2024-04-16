// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AACharacterStat.h"
#include "AAGameSingleton.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API UAAGameSingleton : public UObject
{
	GENERATED_BODY()

 //ver0.0.1a
 //this singleton is for CharacterStatComponent
public:
	UAAGameSingleton();
	static UAAGameSingleton& Get();

public:
	FORCEINLINE FAACharacterStat GetCharacterStat(int32 InLevel) const { return CharacterStatTable.IsValidIndex(InLevel - 1) ? CharacterStatTable[InLevel - 1] : FAACharacterStat(); }

	UPROPERTY()
	int32 CharacterMaxLevel;

private:
	TArray<FAACharacterStat> CharacterStatTable;
};
