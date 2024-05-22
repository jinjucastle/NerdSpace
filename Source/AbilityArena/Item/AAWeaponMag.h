// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AAWeaponMag.generated.h"

UCLASS()
class ABILITYARENA_API AAAWeaponMag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAAWeaponMag();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Properties, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MagMesh;

	UPROPERTY(VisibleAnywhere, Category = Properties, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UProjectileMovementComponent> MagMovement;

	FORCEINLINE void SetMesh(UStaticMesh* NewMesh) { MagMesh->SetStaticMesh(NewMesh); }
};
