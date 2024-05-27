// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AACharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "AACharacterControlData.h"
#include "CharacterStat/AACharacterStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimMontage.h"
#include "Item/AAItemHeader.h"
#include "CharacterStat/AACharacterPlayerState.h"
#include "GameData/AAGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AAPlayerController.h"
#include "EngineUtils.h"

DEFINE_LOG_CATEGORY(LogAACharacter);

// Sets default values
AAACharacterBase::AAACharacterBase()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	GetCapsuleComponent()->SetCapsuleSize(43.f, 95.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -95.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("PhysicsActor"));

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
	Weapon->SetRelativeScale3D(FVector(1.25f, 1.f, 1.25f));
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
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

	// ver 0.7.1a
	MagMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagMesh"));
	MagMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	MagMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MagMeshComponent->SetHiddenInGame(true);

	MagInHandComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagInHandMesh"));
	MagInHandComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	MagInHandComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MagInHandComponent->SetHiddenInGame(true);
}

void AAACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Stat->OnStatChanged.AddUObject(this, &AAACharacterBase::ApplyStat);
	Stat->OnHpZero.AddUObject(this, &AAACharacterBase::SetDead);
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
	SetWeaponDataStore();
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
		if (!HasAuthority())
		{
			WeaponData = WeaponItemData;
			
			SetWeaponMesh(WeaponData);
			
			//ver0.8.1b
			//client saveWeaponData
			SetWeaponDataBegin();
		}

		if(IsLocallyControlled())
		{
			ServerRPCChangeWeapon(WeaponItemData);
		}
	}
}

bool AAACharacterBase::ServerRPCChangeWeapon_Validate(UAAWeaponItemData* NewWeaponData)
{
	return true;
}

void AAACharacterBase::ServerRPCChangeWeapon_Implementation(UAAWeaponItemData* NewWeaponData)
{
	if (NewWeaponData)
	{
		
		
			WeaponData = NewWeaponData;
			SetWeaponMesh(WeaponData);
			Stat->SetWeaponStat(WeaponData->WeaponStat);

			//0.8.1b
			//Server saved Weapondata
			SetWeaponDataBegin();
			UE_LOG(LogAACharacter, Error, TEXT("TestPoint"));
			// ver 0.3.2a
			// Set Ammo Size
			MaxAmmoSize = WeaponData->AmmoPoolExpandSize;
			CurrentAmmoSize = MaxAmmoSize;
		
	}

	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				AAACharacterBase* OtherPlayer = Cast<AAACharacterBase>(PlayerController->GetPawn());
				if (OtherPlayer)
				{
					OtherPlayer->ClientRPCChangeWeapon(this, WeaponData);
				}
			}
		}
	}
}

void AAACharacterBase::ClientRPCChangeWeapon_Implementation(AAACharacterBase* CharacterToPlay, UAAWeaponItemData* NewWeaponData)
{
	if (CharacterToPlay)
	{
		CharacterToPlay->EquipWeapon(NewWeaponData);
	}
}

void AAACharacterBase::SetWeaponMesh(UAAWeaponItemData* NewWeaponData)
{
	if (WeaponData->WeaponMesh.IsPending())
	{
		WeaponData->WeaponMesh.LoadSynchronous();
	}
	Weapon->SetSkeletalMesh(WeaponData->WeaponMesh.Get());

	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("DEF-hand_RSocket"));
	FTransform SocketWorldTransform = Weapon->GetSocketTransform("Hand_R_Pos", RTS_World);
	FTransform ComponentWorldTransform = Weapon->GetComponentTransform();
	FTransform SocketRelativeTransform = SocketWorldTransform.GetRelativeTransform(ComponentWorldTransform);
	Weapon->SetRelativeLocation(SocketRelativeTransform.GetLocation());

	if (WeaponData->ShellMesh.IsPending())
	{
		WeaponData->ShellMesh.LoadSynchronous();
	}

	if (WeaponData->MagMesh.IsPending())
	{
		WeaponData->MagMesh.LoadSynchronous();
	}
	MagMeshComponent->SetStaticMesh(WeaponData->MagMesh.Get());
	MagMeshComponent->AttachToComponent(Weapon, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("MagSocket"));
	if (WeaponData->Type != EWeaponType::Shotgun)
	{
		MagMeshComponent->SetHiddenInGame(false);
	}
	else
	{
		MagMeshComponent->SetHiddenInGame(true);
	}

	MagInHandComponent->SetStaticMesh(WeaponData->MagMesh.Get());
	MagInHandComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("DEF-hand_LSocket"));
}
void AAACharacterBase::SetWeaponDataBegin()
{
	//ver0.8.1b
	//SavedWeaponData 
	if (IsLocallyControlled())
	{
		AAAPlayerController* testController = Cast<AAAPlayerController>(GetController());
		if (testController)
		{
			UAAGameInstance* PC = Cast<UAAGameInstance>(testController->GetGameInstance());
			PC->SetWeaponItemData(WeaponData);
			//UE_LOG(LogAACharacter, Error, TEXT("WeaponData:%s"), *PC->GetName());

		}
	}
	
}

void AAACharacterBase::SetWeaponDataStore()
{
	//ver0.8.1b
	// bring in WeaponData
	AAAPlayerController* testController = Cast<AAAPlayerController>(GetController());
	if (testController)
	{
		UAAGameInstance* PC = Cast<UAAGameInstance>(testController->GetGameInstance());
		
		if (PC->GetsetWeaponItemData())
		{
			WeaponData = testController->SetInitData();
			//UE_LOG(LogAACharacter, Error, TEXT("WeaponData:%s"), *WeaponData->GetName());
		}
		else
		{
			UE_LOG(LogAACharacter, Error, TEXT("Point"));
		}


	}
}

void AAACharacterBase::PlayReloadAnimation()
{
	if (ReloadMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->StopAllMontages(0.1f);
			AnimInstance->Montage_Play(ReloadMontage, ReloadSpeed);
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AAACharacterBase::ReloadActionEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, ReloadMontage);

			ServerSetCanFire(false);

			UE_LOG(LogTemp, Warning, TEXT("[%s] Current Ammo Size : %d"), *GetName(), CurrentAmmoSize);		
			
		}
	}

}

void AAACharacterBase::ReloadActionEnded(UAnimMontage* Montage, bool IsEnded)
{
	if (HasAuthority())
	{
		ServerSetCanFire(true);

		CurrentAmmoSize = FMath::Clamp(CurrentAmmoSize + MaxAmmoSize, 0, MaxAmmoSize);
	}
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

void AAACharacterBase::SpawnShell(FTransform InSocketTransform)
{
	if (ShellClass)
	{
		AAAWeaponShell* Shell = GetWorld()->SpawnActor<AAAWeaponShell>(ShellClass, InSocketTransform);
		Shell->SetMesh(WeaponData->ShellMesh.Get());
		Shell->SetLifeSpan(2.0f);
		Shell->ShellMesh->SetSimulatePhysics(true);

		FVector ImpulseForward = Shell->GetActorForwardVector() * FMath::RandRange(1.f, 3.f);
		FVector ImpulseRight = Shell->GetActorRightVector() * FMath::RandRange(-3.f, -9.f);
		FVector ImpulseResult = ImpulseForward + ImpulseRight;

		Shell->ShellMesh->AddImpulse(ImpulseResult);
	}
}

void AAACharacterBase::ChangeMagazine()
{
	if (WeaponData && WeaponData->MagMesh.IsValid() && MagClass)
	{
		MagInHandComponent->SetHiddenInGame(false);
	}
}

void AAACharacterBase::DropMagazine()
{
	if (WeaponData && WeaponData->MagMesh.IsValid() && MagClass)
	{
		FTransform MagSocketTransForm = Weapon->GetSocketTransform(TEXT("MagSocket"));

		if (WeaponData->Type == EWeaponType::Panzerfaust)
		{
			//TODO : Add RPG Reload Motion
			MagMeshComponent->SetHiddenInGame(true);
		}
		else if (WeaponData->Type == EWeaponType::Shotgun)
		{
			MagMeshComponent->SetHiddenInGame(true);

			for (int i = 0; i < 2; i++)
			{
				AAAWeaponMag* DropMag = GetWorld()->SpawnActor<AAAWeaponMag>(MagClass, MagSocketTransForm);
				DropMag->SetMesh(WeaponData->MagMesh.Get());
				DropMag->SetLifeSpan(2.0f);
				DropMag->MagMesh->SetSimulatePhysics(true);
				FVector ImpulseForward = DropMag->GetActorForwardVector() * FMath::RandRange(1.f, 3.f);
				FVector ImpulseRight = DropMag->GetActorRightVector() * FMath::RandRange(-3.f, -9.f);
				FVector ImpulseResult = ImpulseForward + ImpulseRight;
				DropMag->MagMesh->AddImpulse(ImpulseResult);
			}
		}
		else
		{
			MagMeshComponent->SetHiddenInGame(true);

			AAAWeaponMag* DropMag = GetWorld()->SpawnActor<AAAWeaponMag>(MagClass, MagSocketTransForm);
			DropMag->SetMesh(WeaponData->MagMesh.Get());
			DropMag->SetLifeSpan(2.0f);
			DropMag->MagMesh->SetSimulatePhysics(true);
			DropMag->MagMesh->AddImpulse(FVector::ZeroVector);
		}
	}
}

void AAACharacterBase::AttachNewMagazine()
{
	if (WeaponData && WeaponData->MagMesh.IsValid() && MagClass)
	{
		if (WeaponData->Type != EWeaponType::Shotgun)
		{
			MagMeshComponent->SetHiddenInGame(false);
		}
		MagInHandComponent->SetHiddenInGame(true);
	}
}

float AAACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.f)
	{
		Stat->ApplyDamage(ActualDamage);
	}

	return ActualDamage;
}

void AAACharacterBase::BloodDrain(const float Damage)
{
	int32 NewHp = (int32)(Stat->GetCurrentHp() + Damage * 0.1);

	if (NewHp <= 0) NewHp = 1;

	Stat->SetHp(NewHp);
}

void AAACharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
