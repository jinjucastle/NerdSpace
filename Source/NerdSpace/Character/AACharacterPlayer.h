// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AACharacterBase.h"
#include "InputActionValue.h"
#include "Engine/StreamableManager.h"
#include "AACharacterPlayer.generated.h"

/**
 * 
 */
UCLASS(config=NerdSpace)
class NERDSPACE_API AAACharacterPlayer : public AAACharacterBase
{
	GENERATED_BODY()
	
public:
	AAACharacterPlayer();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

// Character Control Section
public:
	void ChangeZoom();

protected:
	void SetCharacterControl(ECharacterZoomType NewCharacterZoomType);
	virtual void SetCharacterControlData(const class UAACharacterControlData* CharacterControlData) override;
	virtual void PostInitializeComponents()override;


// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	// ver 0.10.3a
	// SniferRifle Zoom
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float DefaultFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float ZoomedFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float ZoomInterpSpeed;

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

	// ver 0.14.3a
	// Input Options
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PauseAction;
	
	//ver 0.3.2a
	//Add Reload Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReloadAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Run();
	void StopRun();

	// ver 0.14.3a
	// Pause
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UUserWidget> PauseWidgetClass;

	TObjectPtr<class UUserWidget> PauseWidgetInstance;

	void ShowPauseUI();

	UFUNCTION(BlueprintCallable)
	void HidePauseUI();

public:
	void RemoveAllUI();

	UFUNCTION(Client, Unreliable)
	void ClientRPCRemoveAllUI();

protected:
	//ver 0.4.1 C
	void StartJump();

	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCRun();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCStopRun();

	bool bIsJump;
	bool bIsRun;

	UPROPERTY(BlueprintReadOnly)
	ECharacterZoomType CurrentCharacterZoomType;

public:
	FORCEINLINE ECharacterZoomType GetCurrentCharacterZoomType() { return CurrentCharacterZoomType; }

// ver 0.0.2a
// Object Pool
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AmmoPool, Meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class AAAWeaponAmmo>> AmmoPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AmmoPool, Meta = (AllowPrivateAccess = "true"))
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

	// ver 0.10.3a
	// New Fire Direction
	FVector GetAdjustedAim()const;

	// ver 0.11.2a
	// Fire Spread System
	FVector GetMovementSpreadDirection(const FVector& InAimDirection) const;

private:
	FTimerHandle TimerHandle_AutomaticFire;
	float FireRate;

// ver 0.1.1a
// Listen Server Projectile section
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCFire(const FVector& NewLocation, const FVector& NewDirection);

	UFUNCTION(Client, Unreliable)
	void ClientRPCSpawnShell();

// ver 0.1.3a
// AmmoMesh & AmmoStat Replicate
public:
	void SetPooledAmmoClass(class UClass* NewAmmoClass);

	UFUNCTION(BlueprintCallable)
	void EquipAmmo(class UClass* NewAmmoClass);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCSetPooledAmmoClass(class UClass* NewAmmoClass);

	UFUNCTION(Client, Unreliable)
	void ClientRPCSetPooledAmmoClass(AAACharacterPlayer* CharacterToPlay, UClass* NewAmmoClass);

// ver 0.3.2a
// Add Reload Action
public:
	void Reload();

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCPlayReloadAnimation();

	UFUNCTION(Client, Unreliable)
	void ClientRPCPlayReloadAnimation(AAACharacterPlayer* CharacterToPlay);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCPlayEmptyReloadAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlayReloadAnimation();

// ver 0.3.4a
// Add Fire Delay
	float NextFireTime;

// ver 0.3.4a
// Add Shotgun spread system
	FRotator GetRandomRotator();

//ver 0.4.1 C
//is running?
protected:
	bool bIsFiring;

// ver 0.6.2a
// AbilityStat Section
protected:
	UPROPERTY(VisibleInstanceOnly, Category = Ability, Meta = (AllowPrivateAccess = "true"))
	FAAAbilityStat SelectedAbility;

	UPROPERTY(Config, EditAnywhere)
	TArray<FSoftObjectPath> CharacterMesh;

	TSharedPtr<FStreamableHandle> CharacterMeshHandle;

public:
	UFUNCTION(BlueprintCallable)
	void SetAbility(const FAAAbilityStat& InAddAbility);

	FORCEINLINE const FAAAbilityStat& GetAbilityStat() const { return SelectedAbility; }

	UFUNCTION(BlueprintCallable)
	void ApplyAbility();

	UFUNCTION(Server, Reliable)
	void ServerRPCApplyAbility(const FAAAbilityStat& NewAbilityStat);

	UFUNCTION(Client, Reliable)
	void ClientRPCApplyAbility(AAACharacterPlayer* CharacterToPlay, const FAAAbilityStat& NewAbilityStat);

	void SetAllAbility(const FAAAbilityStat& NewAbilityStat);
	
	void SetAbilityInController(const FAAAbilityStat& NewAbilityStat);

	void SetAbilityBeginPlay();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSyncAbility();

	UFUNCTION(BlueprintCallable)
	USkeletalMesh* SetChangeSkeletalMesh(bool bChange);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 CurrentIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxIndex;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurrentIndex() { return CurrentIndex; }

	UFUNCTION(BlueprintCallable)
	USkeletalMesh* updateSkeletalMesh(int32 NewIndex);

//Add extra stat
protected:
	float AmmoScale = 1.f;
	float Acceleration;
	float SplashRound = 1.f;
	bool bBloodDrain;
	int32 Magnification;

public:
	FORCEINLINE const float GetAcceleration() const { return Acceleration; }
	FORCEINLINE const float GetAmmoScale() const { return AmmoScale; }
	FORCEINLINE const float GetSplashRound() const { return SplashRound; }
	FORCEINLINE const bool GetCanBloodDrain() const { return bBloodDrain; }

// ver 0.7.3a
// Camera Filter
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PostProcess)
	TObjectPtr<class UPostProcessComponent> PostProcessComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PostProcess)
	TObjectPtr<class UMaterialInterface> DotEffectMaterial;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAmmoDamage() const { return AmmoDamage; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAmmoSpeed() const { return AmmoSpeed; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetRPM() const { return RPM; }

// ver 0.10.1a
// Card Select Time Set Plyer Can't Fire
	void SetPlayerStopFire();

// ver 0.10.3a
// Snifer Rifle Scope UI Section
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UUserWidget> ScopeWidgetClass;

	TObjectPtr<class UUserWidget> ScopeWidgetInstance;

	void ShowScopeWidget();
	void HideScopeWidget();

// ver 0.14.4a
// Mouse Sensitive
	UPROPERTY(BlueprintReadWrite, Category = Settings)
	float NormalSensitiveX;

	UPROPERTY(BlueprintReadWrite, Category = Settings)
	float NormalSensitiveY;

	UPROPERTY(BlueprintReadWrite, Category = Settings)
	float ZoomInSensitiveX;

	UPROPERTY(BlueprintReadWrite, Category = Settings)
	float ZoomInSensitiveY;

// ver 0.15.2a
// Spectator Camera Section
	virtual void SetDead() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Spectator)
	TSubclassOf<class ASpectatorPawn> SpectatorCamera;

// ver 0.17.3a
// Slow Event
public:
	UFUNCTION(BlueprintCallable)
	void ApplySlow();

	UFUNCTION(BlueprintCallable)
	void EndSlow();

protected:
	UPROPERTY(Replicated)
	bool bIsSlowly;
};
