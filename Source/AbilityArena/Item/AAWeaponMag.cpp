// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AAWeaponMag.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AAAWeaponMag::AAAWeaponMag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagMesh"));
	RootComponent = MagMesh;

	MagMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	MagMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MagMovement"));
	MagMovement->SetUpdatedComponent(RootComponent);
	MagMovement->bShouldBounce = true;
}

// Called when the game starts or when spawned
void AAAWeaponMag::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAAWeaponMag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

