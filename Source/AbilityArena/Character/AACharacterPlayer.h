// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AACharacterBase.h"
#include "InputActionValue.h"
#include "AACharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API AAACharacterPlayer : public AAACharacterBase
{
	GENERATED_BODY()
	
public:
	AAACharacterPlayer();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

// Character Control Section
protected:
	void ChangeZoom();
	void SetCharacterControl(ECharacterZoomType NewCharacterZoomType);
	virtual void SetCharacterControlData(const class UAACharacterControlData* CharacterControlData) override;

// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ChangeZoomAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireStopAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RunAction;
	
	//ver 0.3.2a
	//Add Reload Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReloadAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Run();
	void StopRun();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCRun();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCStopRun();

	bool bIsRun;

	ECharacterZoomType CurrentCharacterZoomType;

// ver 0.0.2a
// Object Pool
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AmmoPool, Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class AAAWeaponAmmo>> AmmoPool;

	UPROPERTY(ReplicatedUsing = OnRep_PooledAmmoClass, EditAnywhere, BlueprintReadWrite, Category = AmmoPool, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AAAWeaponAmmo> PooledAmmoClass;

	int32 AmmoPoolSize = 0;
	int32 AmmoExpandSize = 0;

public:
	class AAAWeaponAmmo* GetPooledAmmo();
	void Expand();
	void ReturnAmmo(class AAAWeaponAmmo* InReturnAmmoActor);

	UFUNCTION(BlueprintCallable)
	void ClearPool();

// ver 0.0.2a
// Fire Action
public:
	void Fire();

	// ver 0.3.3a
	// Add Fire Rate System
	void StartFire();
	void StopFire();

private:
	FTimerHandle TimerHandle_AutomaticFire;
	float FireRate;

// ver 0.1.1a
// Listen Server Projectile section
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCFire(const FVector& NewLocation, const FRotator& NewRotation);

// ver 0.1.3a
// AmmoMesh & AmmoStat Replicate
public:
	void SetPooledAmmoClass(class UClass* NewAmmoClass);

	UFUNCTION(BlueprintCallable)
	void EquipAmmo(class UClass* NewAmmoClass);

	UFUNCTION()
	void OnRep_PooledAmmoClass();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCSetPooledAmmoClass(class UClass* NewAmmoClass);

// ver 0.3.2a
// Add Reload Action
public:
	void Reload();

protected:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPlayReloadAnimation();

// ver 0.3.4a
// Add Fire Delay
	float NextFireTime;

// ver 0.3.4a
// Add Shotgun spread system
	FRotator GetRandomRotator();
};
