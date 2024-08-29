// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AASpawnPoint.h"

// Sets default values
AAASpawnPoint::AAASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsPlayerStart = true;
}

// Called when the game starts or when spawned
void AAASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAASpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

