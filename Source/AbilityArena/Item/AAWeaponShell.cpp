// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AAWeaponShell.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

// Sets default values
AAAWeaponShell::AAAWeaponShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShellMesh"));
	RootComponent = ShellMesh;

	ShellMesh->SetRelativeRotation(FRotator(-90.f, -90.f, 0.f));
	ShellMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ShellMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ShellMovement"));
	ShellMovement->SetUpdatedComponent(RootComponent);
	ShellMovement->bShouldBounce = true;

	ShellRotating = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("ShellRotating"));
	ShellRotating->SetUpdatedComponent(RootComponent);
	ShellRotating->RotationRate.Pitch = 100.f;
	ShellRotating->RotationRate.Roll = -100.f;
}

// Called when the game starts or when spawned
void AAAWeaponShell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAAWeaponShell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

