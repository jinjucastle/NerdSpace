// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AACharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "AACharacterBase.h"
#include "AACharacterControlData.h"
#include "CharacterStat/AACharacterStatComponent.h"
#include "Item/AAWeaponItemData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
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
}

// Called when the game starts or when spawned
void AAACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
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
	float MovementSpeed = (BaseStat + WeaponStat).MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	AmmoDamage = WeaponData->AmmoDamage;
	AmmoSpeed = WeaponData->AmmoSpeed;
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
	}
	if(!HasAuthority())
	{
		ServerRPCChangeWeapon(WeaponData);
	}
}

void AAACharacterBase::TakeItem(UAAItemData* InItemData)
{
	/*if (InItemData)
	{
		TakeItemActions[(uint8)InItemData->Type].ItemDelegate.ExecuteIfBound(InItemData);
	}*/
}

void AAACharacterBase::RecoverHealth(UAAItemData* InItemData)
{
	UE_LOG(LogAACharacter, Log, TEXT("Read Scroll"));
}

void AAACharacterBase::MakeShield(UAAItemData* InItemData)
{
	UE_LOG(LogAACharacter, Log, TEXT("Read Scroll"));
}
