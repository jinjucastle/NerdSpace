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
#include "Sound/SoundCue.h"

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

	static ConstructorHelpers::FObjectFinder<UAnimMontage> PistolReloadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/Action/AM_Reload_Pistol.AM_Reload_Pistol'"));
	if (PistolReloadMontageRef.Object)
	{
		PistolReloadMontage = PistolReloadMontageRef.Object;
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

	// ver 0.10.1a
	// Recoil setting
	RecoilStrength = 1.0f;
	RecoilRecoverySpeed = 5.0f;
	CurrentRecoil = FRotator::ZeroRotator;
	TargetRecoil = FRotator::ZeroRotator;
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
		}
	}
}

void AAACharacterBase::PlayReloadAnimation()
{
	if (ReloadMontage || PistolReloadMontage)
	{
		FOnMontageEnded EndDelegate;

		if (WeaponData->Type == EWeaponType::Pistol)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->StopAllMontages(0.1f);
				AnimInstance->Montage_Play(PistolReloadMontage, ReloadSpeed);
				EndDelegate.BindUObject(this, &AAACharacterBase::ReloadActionEnded);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, PistolReloadMontage);
			}
		}
		else
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->StopAllMontages(0.1f);
				AnimInstance->Montage_Play(ReloadMontage, ReloadSpeed);
				EndDelegate.BindUObject(this, &AAACharacterBase::ReloadActionEnded);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, ReloadMontage);
			}
		}
		ServerSetCanFire(false);

		UE_LOG(LogTemp, Warning, TEXT("[%s] Current Ammo Size : %d"), *GetName(), CurrentAmmoSize);
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

		FTimerHandle ShellTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			ShellTimerHandle,
			FTimerDelegate::CreateLambda([this]() {
				PlayBoundShellSound();
				}), 0.3f, false);
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
		if (ActualDamage > 30.f)
		{
			PlaySound(LargeHitSoundCue, GetActorLocation());
		}
		else if (ActualDamage > 15.f)
		{
			PlaySound(MidiumHitSoundCue, GetActorLocation());
		}
		else
		{
			PlaySound(SmallHitSoundCue, GetActorLocation());
		}
	}


	if (AAAPlayerController* InstigatorController = Cast<AAAPlayerController>(EventInstigator))
	{
		if (AAACharacterBase* InstigatingCharacter = Cast<AAACharacterBase>(InstigatorController->GetPawn()))
		{
			InstigatingCharacter->ClientRPCPlayHitSuccessSound();
			UE_LOG(LogTemp, Warning, TEXT("Controller : %s, Pawn : %s"), *InstigatorController->GetName(), *InstigatingCharacter->GetName());
		}
	}

	return ActualDamage;
}

void AAACharacterBase::BloodDrain(const float Damage)
{
	int32 NewHp = (int32)(Stat->GetCurrentHp() + Damage * 0.5);

	if (Damage * 0.5 <= 1) NewHp = Stat->GetCurrentHp() + 1;

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

bool AAACharacterBase::ServerRPCPlaySound_Validate(USoundCue* SoundCue, FVector Location)
{
	return true;
}

void AAACharacterBase::ServerRPCPlaySound_Implementation(USoundCue* SoundCue, FVector Location)
{
	MulticastRPCPlaySound(SoundCue, Location);
}

void AAACharacterBase::MulticastRPCPlaySound_Implementation(USoundCue* SoundCue, FVector Location)
{
	if (SoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundCue, Location);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Multicast PlaySound Cue is null"));
	}
}

void AAACharacterBase::PlayBoundShellSound()
{
	if (ShellDropSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellDropSoundCue, GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ShellDropSoundCue is null"));
	}
}

void AAACharacterBase::ClientRPCPlayHitSuccessSound_Implementation()
{
	if (SuccessHitSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SuccessHitSoundCue, GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SuccessHitSoundCue is null"));
	}
}

void AAACharacterBase::PlaySound(USoundCue* InSoundCue, FVector InLocation)
{
	if (InSoundCue)
	{
		if (HasAuthority())
		{
			MulticastRPCPlaySound(InSoundCue, InLocation);
		}
		else
		{
			ServerRPCPlaySound(InSoundCue, InLocation);
		}
	}
}

void AAACharacterBase::PlayFootSound()
{
	if (FootStepSoundCue)
	{
		PlaySound(FootStepSoundCue, GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FootStepSoundCue is null"));
	}
}

void AAACharacterBase::PlayRemoveMagSound()
{
	switch (WeaponData->Type)
	{
	case EWeaponType::Shotgun:
		if (SGMagDropSoundCue) PlaySound(SGMagDropSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("SGMagDropSoundCue is null"));
		break;
	case EWeaponType::Panzerfaust:
		if (RPGMagDropSoundCue) PlaySound(RPGMagDropSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("RPGMagDropSoundCue is null"));
		break;
	default:
		if(DefMagDropSoundCue) PlaySound(DefMagDropSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("DefMagDropSoundCue is null"));
		break;
	}
}

void AAACharacterBase::PlayInsertMagSound()
{
	switch (WeaponData->Type)
	{
	case EWeaponType::Pistol:
		if (PSTMagInsertSoundCue) PlaySound(PSTMagInsertSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("PSTMagInsertSoundCue is null"));
		break;
	case EWeaponType::Shotgun:
		if (SGMagInsertSoundCue) PlaySound(SGMagInsertSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("SGMagInsertSoundCue is null"));
		break;
	case EWeaponType::Panzerfaust:
		if (RPGMagInsertSoundCue) PlaySound(RPGMagInsertSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("RPGMagInsertSoundCue is null"));
		break;
	default:
		if (ARandSRMagInsertSoundCue) PlaySound(ARandSRMagInsertSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("ARandSRMagInsertSoundCue is null"));
		break;
	}
}

void AAACharacterBase::PlayCockingSound()
{
	switch (WeaponData->Type)
	{
	case EWeaponType::Pistol:
		if (PSTCockingSoundCue) PlaySound(PSTCockingSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("PSTCockingSoundCue is null"));
		break;
	case EWeaponType::Rifle:
		if (ARCockingSoundCue) PlaySound(ARCockingSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("ARCockingSoundCue is null"));
		break;
	case EWeaponType::Shotgun:
		if (SGCockingSoundCue) PlaySound(SGCockingSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("SGCockingSoundCue is null"));
		break;
	case EWeaponType::Panzerfaust:
		if (RPGCockingSoundCue) PlaySound(RPGCockingSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("RPGCockingSoundCue is null"));
		break;
	default:
		if (SRCockingSoundCue) PlaySound(SRCockingSoundCue, GetActorLocation());
		else UE_LOG(LogTemp, Error, TEXT("SRCockingSoundCue is null"));
		break;
	}
}

void AAACharacterBase::ApplyRecoil(float Damage)
{
	float RecoilPitch = 1.f * Damage * RecoilStrength;
	float RecoilYaw = FMath::RandRange(-1.0f, 1.0f) * RecoilStrength;

	TargetRecoil.Pitch = RecoilPitch;
	TargetRecoil.Yaw = RecoilYaw;

	CurrentRecoil = FMath::RInterpTo(CurrentRecoil, TargetRecoil, GetWorld()->GetDeltaSeconds(), RecoilRecoverySpeed);

	UE_LOG(LogTemp, Log, TEXT("Yaw : %f, Pitch : %f"), CurrentRecoil.Yaw, CurrentRecoil.Pitch);

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AAACharacterBase::RecoverRecoil);
}

void AAACharacterBase::RecoverRecoil()
{
	if (CurrentRecoil.IsNearlyZero())
	{
		return;
	}

	CurrentRecoil = FMath::RInterpTo(CurrentRecoil, FRotator::ZeroRotator, GetWorld()->GetDeltaSeconds(), RecoilRecoverySpeed);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		FRotator NewControlRotation = PlayerController->GetControlRotation();
		NewControlRotation += CurrentRecoil;
		PlayerController->SetControlRotation(NewControlRotation);
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AAACharacterBase::RecoverRecoil);
}

