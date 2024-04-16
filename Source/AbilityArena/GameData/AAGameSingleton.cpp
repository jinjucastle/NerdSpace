// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/AAGameSingleton.h"

UAAGameSingleton::UAAGameSingleton()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableRef(TEXT("/Script/Engine.DataTable'/Game/GameData/AACharacterStatTable.AACharacterStatTable'"));
	if (nullptr != DataTableRef.Object)
	{
		const UDataTable* DataTable = DataTableRef.Object;
		check(DataTable->GetRowMap().Num() > 0);

		TArray<uint8*> ValueArray;
		DataTable->GetRowMap().GenerateValueArray(ValueArray);
		Algo::Transform(ValueArray, CharacterStatTable,
			[](uint8* Value)
			{
				return *reinterpret_cast<FAACharacterStat*>(Value);
			}
		);
	}

	CharacterMaxLevel = CharacterStatTable.Num();
	ensure(CharacterMaxLevel > 0);
}

UAAGameSingleton& UAAGameSingleton::Get()
{
	UAAGameSingleton* Singleton = CastChecked< UAAGameSingleton>(GEngine->GameSingleton);
	if (Singleton)
	{
		return *Singleton;
	}

	UE_LOG(LogTemp, Error, TEXT("Invalid Game Singleton"));
	return *NewObject<UAAGameSingleton>();
}
