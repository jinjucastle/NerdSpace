// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameData/AACharacterStat.h"
#include "GameData/AAAbilityStat.h"
#include "Interface/AACharacterItemInterface.h"
#include "AACharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAACharacter, Log, All);

UENUM()
enum class ECharacterZoomType : uint8
{
	ZoomOut,
	ZoomIn
};

//ver 0.3.0 C
DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class UAAItemData* /*InItemData*/);
USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()
	FTakeItemDelegateWrapper() {}
	FTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate) :ItemDelegate(InItemDelegate) {}

	FOnTakeItemDelegate ItemDelegate;
};

UCLASS()
class ABILITYARENA_API AAACharacterBase : public ACharacter,public IAACharacterItemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAACharacterBase();

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetCharacterControlData(const class UAACharacterControlData* CharacterControlData);
	

	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterZoomType, class UAACharacterControlData*> CharacterControlManager;

// ver 0.0.1a
// Stat Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAACharacterStatComponent> Stat;

public:
	void ApplyStat(const FAACharacterStat& BaseStat, const FAACharacterStat& WeaponStat);

// ver 0.0.1a
// Item Section
protected:
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(class UAAItemData* InItemData);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Weapon;

	// ver 0.1.2a
	// Set Replicated
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAAWeaponItemData> WeaponData;

	//ver 0.4.2b
	//GameInstance 
	TObjectPtr<class UAAGameInstance> gameInstance;
	TObjectPtr<class AAACharacterPlayerState>playerState;
// ver 0.3.0 C
	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions;

	virtual void TakeItem(class UAAItemData* InItemData) override;
	virtual void RecoverHealth(class UAAItemData* InItemData);
	virtual void MakeShield(class UAAItemData* InItemData);


// ver 0.0.2a
// WeaponData Get
protected:
	float AmmoDamage;
	float AmmoSpeed;

// ver 0.1.2a
// Replicated
protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCChangeWeapon(class UAAWeaponItemData* NewWeaponData);

	UFUNCTION(Client, Reliable)
	void ClientRPCChangeWeapon(AAACharacterBase* CharacterToPlay, class UAAWeaponItemData* NewWeaponData);

	void SetWeaponMesh(class UAAWeaponItemData* NewWeaponData);
public:
	void SetWeaponDataBegin();
	void SetWeaponDataStore();

	// ver 0.18.1a
	// Server Replicate WeaponData
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCSetWeaponDataStore(class UAAWeaponItemData* NewWeaponData);

	UFUNCTION(Client, UnReliable)
	void ClientRPCSetWeaponDataStore(class UAAWeaponItemData* NewWeaponData, AAACharacterBase* CharacterToPlay);

// ver 0.3.2a
// AmmoSize
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 MaxAmmoSize = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 CurrentAmmoSize = 0;

// ver 0.3.2a
// Add Reload Motion
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> ReloadMontage;

	// ver 0.11.2a
	// Add Pistol Reload Motion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> PistolReloadMontage;

	UPROPERTY(Replicated)
	bool bCanFire;

public:
	void PlayReloadAnimation();
	void ServerSetCanFire(bool NewCanFire);
	void CompleteReload();
  
  //TODO : Delete
	int32 TestNum;

// ver 0.6.2a
// Add MovementSpeed & RPM Storage
protected:
	float RPM = 1.f;
	float BaseMovementSpeed;
	float ReloadSpeed = 1.f;

public:


// ver 0.6.4a
// UI Section
public:
	FORCEINLINE class UAAWeaponItemData* GetWeaponData() const { return WeaponData; }

// ver 0.7.1a
// Add Ammo Shell
protected:
	UPROPERTY(EditAnywhere, Category = Ammo, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AAAWeaponShell> ShellClass;

public:
	void SpawnShell(FTransform InSocketTransform);

// Magazine Section
protected:
	UPROPERTY(EditDefaultsOnly, Category = Bullet)
	TObjectPtr<UStaticMeshComponent> MagMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = Bullet)
	TObjectPtr<UStaticMeshComponent> MagInHandComponent;

	UPROPERTY(EditAnywhere, Category = Ammo, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AAAWeaponMag> MagClass;

public:
	void ChangeMagazine();
	void DropMagazine();
	void AttachNewMagazine();

// ver 0.7.3a
// Damage Section
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void BloodDrain(const float Damage);

// ver 0.7.4a
// Dead Section
	virtual void SetDead();

// ver 0.9.3a
// Sound Section
protected:
	UPROPERTY(EditDefaultsOnly, Category = "FireAudio")
	TObjectPtr<class USoundCue> PSTFireSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "FireAudio")
	TObjectPtr<class USoundCue> ARFireSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "FireAudio")
	TObjectPtr<class USoundCue> SRFireSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "FireAudio")
	TObjectPtr<class USoundCue> SGFireSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "FireAudio")
	TObjectPtr<class USoundCue> RPGFireSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> ShellDropSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> DefMagDropSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> SGMagDropSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> RPGMagDropSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> PSTMagInsertSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> ARandSRMagInsertSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> SGMagInsertSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> RPGMagInsertSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> PSTCockingSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> ARCockingSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> SRCockingSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> SGCockingSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> RPGCockingSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> MagEmptySoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> JumpSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> LandSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> FootStepSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "SFXAudio")
	TObjectPtr<class USoundCue> SuccessHitSoundCue;

public:
	UPROPERTY(EditDefaultsOnly, Category = "VoiceAudio")
	TObjectPtr<class USoundCue> SmallHitSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "VoiceAudio")
	TObjectPtr<class USoundCue> MidiumHitSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "VoiceAudio")
	TObjectPtr<class USoundCue> LargeHitSoundCue;

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCPlaySound(USoundCue* SoundCue, FVector Location);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlaySound(USoundCue* SoundCue, FVector Location);

	void PlayBoundShellSound();

	UFUNCTION(Client, Reliable)
	void ClientRPCPlayHitSuccessSound();

public:
	void PlaySound(class USoundCue* InSoundCue, FVector InLocation);
	void PlayFootSound();
	void PlayRemoveMagSound();
	void PlayInsertMagSound();
	void PlayCockingSound();

	// ver 0.10.1a
	// Add Recoil System
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilStrength;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilRecoverySpeed;

	UPROPERTY(VisibleAnywhere, Category = "Recoil")
	FRotator CurrentRecoil;

	UPROPERTY(VisibleAnywhere, Category = "Recoil")
	FRotator TargetRecoil;

	void ApplyRecoil(float Damage);
	void RecoverRecoil();

// ver 0.11.6a
// Alive check
	UPROPERTY(Replicated)
	bool bIsAlive;

public:
	FORCEINLINE bool GetIsAlive() const { return bIsAlive; }
	float GetHpPercentage() const;

// ver 0.13.5a
// HpBar Section
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> HpBar;

// ver 0.15.4a
// Winner is unDying
	UPROPERTY(Replicated)
	bool bInvincibility = false;

public:
	FORCEINLINE void SetInvincibility() { bInvincibility = true; }
};
