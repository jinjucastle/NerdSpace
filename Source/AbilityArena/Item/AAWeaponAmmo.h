// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AAWeaponAmmo.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Normal = 0,
	Rocket,
	Funny
};

UCLASS()
class ABILITYARENA_API AAAWeaponAmmo : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAAWeaponAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// ver 0.0.2a
// Create Ammo Actior
protected:
	UPROPERTY()
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere, Category = Properties, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> AmmoMesh;

	UPROPERTY(VisibleAnywhere, Category = Properties, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UProjectileMovementComponent> AmmoMovement;

	float Damage;

	//test function
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;


// ver 0.0.2a
// Fire Action 
public:
	// ver 0.3.1a
	// Change Function Parameter
	void Fire()const;

// ver 0.0.2a
// Object Pool System
public:
	void SetOwnerPlayer(class AAACharacterPlayer* InPlayer);
	void ReturnSelf();
	void SetActive(bool InIsActive);
	FORCEINLINE bool IsActive() { return bIsActive; }

private:
	FTimerHandle ActiveHandle;
	bool bIsActive;
	TObjectPtr<class AAACharacterPlayer> Owner;

// ver 0.1.3a
// Add Rocket Acceleration To Need AmmoType
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EAmmoType AmmoType;

// ver 0.3.1a
// Add Ammo Location & Rotation Info
//protected:
//	UPROPERTY(ReplicatedUsing = OnRep_RotationUpdated)
//	FRotator ReplicatedRotation;
//
//public:
//	UFUNCTION()
//	void OnRep_RotationUpdated();
//
//	FORCEINLINE void SetReplicatedRotation(const FRotator NewRotation) { ReplicatedRotation = NewRotation; };
};
