// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AAWeaponShell.generated.h"

UCLASS()
class NERDSPACE_API AAAWeaponShell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAAWeaponShell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Properties, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ShellMesh;

	UPROPERTY(VisibleAnywhere, Category = Properties, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UProjectileMovementComponent> ShellMovement;

	UPROPERTY(VisibleAnywhere, Category = Properties, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class URotatingMovementComponent> ShellRotating;

	FORCEINLINE void SetMesh(UStaticMesh* NewMesh) { ShellMesh->SetStaticMesh(NewMesh); }
};
