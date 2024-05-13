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
	TObjectPtr<class UAAGameInstance> GameInstance;
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

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAmmoDamage() const { return AmmoDamage; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAmmoSpeed() const { return AmmoSpeed; }

// ver 0.1.2a
// Replicated
protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCChangeWeapon(class UAAWeaponItemData* NewWeaponData);

	UFUNCTION(Client, Reliable)
	void ClientRPCChangeWeapon(AAACharacterBase* CharacterToPlay, class UAAWeaponItemData* NewWeaponData);

	void SetWeaponMesh(class UAAWeaponItemData* NewWeaponData);

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

	UPROPERTY(Replicated)
	bool bCanFire;

public:
	void PlayReloadAnimation();
	void ReloadActionEnded(UAnimMontage* Montage, bool IsEnded);
	void ServerSetCanFire(bool NewCanFire);

// ver 0.6.2a
// Add MovementSpeed & RPM Storage
protected:
	float BaseMovementSpeed;
	float RPM = 1.f;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetRPM() const { return RPM; }

// ver 0.6.4a
// UI Section
public:
	FORCEINLINE class UAAWeaponItemData* GetWeaponData() const { return WeaponData; }
};
