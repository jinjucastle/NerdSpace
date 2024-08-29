// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"

FString ALobbyGameMode::SetFirstRandomLevel()
{
	FString RandomLevel;

	if (FirstLevelArrary.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("LevelArray is Empty"));
	}
	int32 RandomIndex = FMath::RandRange(0, FirstLevelArrary.Num() - 1);
	
	RandomLevel = FirstLevelArrary[RandomIndex];

	return RandomLevel;

}

void ALobbyGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	
	UE_LOG(LogTemp, Error, TEXT("PostSeamlessTravel"));
	UFunction* TestFunction = this->FindFunction(FName("TestEvent"));

	if (TestFunction==nullptr)
	{
		
		UE_LOG(LogTemp, Error, TEXT("PostSeamlessTravelTest2"));

	}
	else {
		struct FBlank
		{
		};
		FBlank Params;
		this->ProcessEvent(TestFunction, &Params);
	}
}
