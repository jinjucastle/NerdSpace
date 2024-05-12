// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AACharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "AACharacterBase.h"
#include "AACharacterControlData.h"
#include "CharacterStat/AACharacterStatComponent.h"
#include "Item/AAWeaponItemData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimMontage.h"
#include "Item/AAFieldItemData.h"
#include "Item/AARecoveryItem.h"
#include "Item/AAShieldItem.h"
#include "CharacterStat/AACharacterPlayerState.h"
#include "GameData/AAGameInstance.h"
#include "Item/AAItemData.h"

DEFINE_LOG_CATEGORY(LogAACharacter);

// Sets default values
AAACharacterBase::AAACharacterBase()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -98.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Model/Character/0/0.0'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Animation/ABP_AA_Character.ABP_AA_Character_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	// Zoom
	static ConstructorHelpers::FObjectFinder<UAACharacterControlData> ZoomOutDataRef(TEXT("/Script/AbilityArena.AACharacterControlData'/Game/CharacterControl/AAC_ZoomOut.AAC_ZoomOut'"));
	if (ZoomOutDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterZoomType::ZoomOut, ZoomOutDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UAACharacterControlData> ZoomInDataRef(TEXT("/Script/AbilityArena.AACharacterControlData'/Game/CharacterControl/AAC_ZoomIn.AAC_ZoomIn'"));
	if (ZoomInDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterZoomType::ZoomIn, ZoomInDataRef.Object);
	}

	// ver 0.0.1a
	// Stat Component & Weapon Component
	Stat = CreateDefaultSubobject<UAACharacterStatComponent>(TEXT("Stat"));

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("DEF-hand_RSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ver 0.3.2a
	// Reload Montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ReloadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/Action/AM_Reload.AM_Reload'"));
	if (ReloadMontageRef.Object)
	{
		ReloadMontage = ReloadMontageRef.Object;
	}

	bCanFire = true;

	//ver 0.3.0 C
	 //Item Actions
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AAACharacterBase::RecoverHealth)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AAACharacterBase::MakeShield)));
}

void AAACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Stat->OnStatChanged.AddUObject(this, &AAACharacterBase::ApplyStat);
}

void AAACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AAACharacterBase, WeaponData);
	DOREPLIFETIME(AAACharacterBase, MaxAmmoSize);
	DOREPLIFETIME(AAACharacterBase, CurrentAmmoSize);
	DOREPLIFETIME(AAACharacterBase, bCanFire);
	
}

// Called when the game starts or when spawned
void AAACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	//ver 0.4.2b 
	//Casting GameInstance
	GameInstance = Cast<UAAGameInstance>(GetGameInstance());
	//ver 0.5.1b
	//check playerState->PlayerID()
	//충돌가능성 농후 주석처리
	//playerState = Cast<AAACharacterPlayerState>(GetPlayerState());
	
	
	//ver 0.5.1b
	// check PlayerState playerID() and stay WeaponData;
	/*if (playerState->GetPlayerId() == GameInstance->PlayerInfos[0].PlayerID)
	{
		EquipWeapon(GameInstance->GetsetWeaponItemData());
	 }*/
	
	//test
	EquipWeapon(WeaponData);
}

void AAACharacterBase::SetCharacterControlData(const UAACharacterControlData* CharacterControlData)
{
	//Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	//CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void AAACharacterBase::OnRep_WeaponData()
{
	UE_LOG(LogTemp, Error, TEXT("Called OnRep_WeaponData"));
	EquipWeapon(WeaponData);
	
}

bool AAACharacterBase::ServerRPCChangeWeapon_Validate(UAAWeaponItemData* NewWeaponData)
{
	return true;
}

void AAACharacterBase::ServerRPCChangeWeapon_Implementation(UAAWeaponItemData* NewWeaponData)
{
	WeaponData = NewWeaponData;

	OnRep_WeaponData();
}

void AAACharacterBase::MulticastRPCChangeWeapon_Implementation(UAAWeaponItemData* NewWeaponData)
{
	UE_LOG(LogTemp, Error, TEXT("Client : Called ServerRPCChangeWeapon"));
	WeaponData = NewWeaponData;
}

// ver0.0.1a
// Equip Weapon & Apply Stat
void AAACharacterBase::ApplyStat(const FAACharacterStat& BaseStat, const FAACharacterStat& WeaponStat)
{
	BaseMovementSpeed = (BaseStat + WeaponStat).MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	AmmoDamage = WeaponData->AmmoDamage;
	AmmoSpeed = WeaponData->AmmoSpeed;
	RPM = WeaponData->WeaponStat.RPM;
}

void AAACharacterBase::EquipWeapon(UAAItemData* InItemData)
{
	UAAWeaponItemData* WeaponItemData = Cast<UAAWeaponItemData>(InItemData);
	
	
	if (WeaponItemData)
	{
		WeaponData = WeaponItemData;
		if (WeaponData->WeaponMesh.IsPending())
		{
			WeaponData->WeaponMesh.LoadSynchronous();
		}
		Weapon->SetSkeletalMesh(WeaponData->WeaponMesh.Get());
		Stat->SetWeaponStat(WeaponData->WeaponStat);
		// ver 0.4.2b
		//feat: gameInstance data Storage
		if (GameInstance)
		{
			UE_LOG(LogTemp, Error, TEXT("Find GameInstace"));
			GameInstance->SetWeaponItemData(WeaponData);
		}
		

		// ver 0.4.2a
		// Replace Attach Weapon
		FTransform SocketWorldTransform = Weapon->GetSocketTransform("Hand_R_Pos", RTS_World);
		FTransform ComponentWorldTransform = Weapon->GetComponentTransform();
		FTransform SocketRelativeTransform = SocketWorldTransform.GetRelativeTransform(ComponentWorldTransform);
		Weapon->SetRelativeLocation(SocketRelativeTransform.GetLocation());


		// ver 0.3.2a
		// Set Ammo Size
		MaxAmmoSize = WeaponData->AmmoPoolExpandSize;
		CurrentAmmoSize = MaxAmmoSize;
	}
	if(!HasAuthority())
	{
		ServerRPCChangeWeapon(WeaponData);
	}
}

void AAACharacterBase::PlayReloadAnimation()
{
	if (ReloadMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->StopAllMontages(0.0f);
			AnimInstance->Montage_Play(ReloadMontage, 1.0f);
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AAACharacterBase::ReloadActionEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, ReloadMontage);

			ServerSetCanFire(false);

			CurrentAmmoSize = FMath::Clamp(CurrentAmmoSize + MaxAmmoSize, 0, MaxAmmoSize);

			UE_LOG(LogTemp, Warning, TEXT("[%s] Current Ammo Size : %d"), *GetName(), CurrentAmmoSize);		
			
		}
	}

}

void AAACharacterBase::ReloadActionEnded(UAnimMontage* Montage, bool IsEnded)
{
	ServerSetCanFire(true);
}

void AAACharacterBase::ServerSetCanFire(bool NewCanFire)
{
	if (HasAuthority())
	{
		bCanFire = NewCanFire;
	}
}

void AAACharacterBase::TakeItem(UAAItemData* InItemData)
{
	UAAFieldItemData* AAFieldItemData = Cast<UAAFieldItemData>(InItemData);

	if (AAFieldItemData)
	{
		TakeItemActions[(uint8)AAFieldItemData->Type].ItemDelegate.ExecuteIfBound(InItemData);
	}
}

void AAACharacterBase::RecoverHealth(UAAItemData* InItemData)
{
	UAARecoveryItem* AARecoveryItem = Cast<UAARecoveryItem>(InItemData);

	if (AARecoveryItem)
	{
		UE_LOG(LogAACharacter, Log, TEXT("Recover Health"));
		Stat->HealHp(AARecoveryItem->RecoveryAmount);
	}

}

void AAACharacterBase::MakeShield(UAAItemData* InItemData)
{
	UAAShieldItem* AAShieldItem = Cast<UAAShieldItem>(InItemData);

	if (AAShieldItem)
	{
		UE_LOG(LogAACharacter, Log, TEXT("Make Shield"));
	}

}
