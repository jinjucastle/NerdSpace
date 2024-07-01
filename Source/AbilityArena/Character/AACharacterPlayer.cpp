// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AACharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AACharacterControlData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterStat/AACharacterStatComponent.h"
#include "Item/AAWeaponAmmo.h"
#include "Item/AAWeaponitemData.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "CharacterStat/AACharacterPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PostProcessComponent.h"
#include "Player/AAPlayerController.h"
#include "GameData/AAGameInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Game/AAGameMode.h"
#include "Blueprint/UserWidget.h"

AAACharacterPlayer::AAACharacterPlayer()
{
	//Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 150.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->SetRelativeLocation(FVector(0.0f, 45.0f, 85.0f));
	FollowCamera->bUsePawnControlRotation = false;

	//Input
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionChangeZoomRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Zoom.IA_Zoom'"));
	if (nullptr != InputActionChangeZoomRef.Object)
	{
		ChangeZoomAction = InputActionChangeZoomRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Move.IA_Move'"));
	if (nullptr != InputActionMoveRef.Object)
	{
		MoveAction = InputActionMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Look.IA_Look'"));
	if (nullptr != InputActionLookRef.Object)
	{
		LookAction = InputActionLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFireRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Fire.IA_Fire'"));
	if (nullptr != InputActionFireRef.Object)
	{
		FireAction = InputActionFireRef.Object;
	}

	// ver 0.3.3a
	// Add FireStop Action
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFireStopRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_FireStop.IA_FireStop'"));
	if (nullptr != InputActionFireStopRef.Object)
	{
		FireStopAction = InputActionFireStopRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionRunRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Run.IA_Run'"));
	if (nullptr != InputActionRunRef.Object)
	{
		RunAction = InputActionRunRef.Object;
	}

	// ver 0.3.2a
	// Add Reload Action
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionReloadRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Reload.IA_Reload'"));
	if (nullptr != InputActionReloadRef.Object)
	{
		ReloadAction = InputActionReloadRef.Object;
	}
	
	CurrentCharacterZoomType = ECharacterZoomType::ZoomOut;

	// ver 0.7.3a
	// Camera Filter
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(FollowCamera);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Script/Engine.Material'/Game/Material/M_DotEffet.M_DotEffet'"));
	if (MaterialFinder.Succeeded())
	{
		DotEffectMaterial = MaterialFinder.Object;
	}

	// ver 0.10.3a
	// Default FOV Value
	DefaultFOV = 90.0f;
	ZoomedFOV = 45.0f;
	ZoomInterpSpeed = 20.0f;

	FollowCamera->FieldOfView = DefaultFOV;
}

void AAACharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}

	SetCharacterControl(CurrentCharacterZoomType);

	if (DotEffectMaterial)
	{
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(DotEffectMaterial, this);
		PostProcessComponent->AddOrUpdateBlendable(DynamicMaterial);
	}

	SetAbilityBeginPlay();
}

void AAACharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float NewZoomedFov = ZoomedFOV / Magnification;

	float TargetFOV = (CurrentCharacterZoomType == ECharacterZoomType::ZoomIn && WeaponData->Type == EWeaponType::SniperRifle) ? NewZoomedFov : DefaultFOV;
	float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	FollowCamera->SetFieldOfView(NewFOV);

	if (!CurrentRecoil.IsNearlyZero())
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController)
		{
			FRotator NewControlRotation = PlayerController->GetControlRotation();
			NewControlRotation += CurrentRecoil;
			PlayerController->SetControlRotation(NewControlRotation);
		}

		CurrentRecoil = FMath::RInterpTo(CurrentRecoil, FRotator::ZeroRotator, DeltaTime, RecoilRecoverySpeed);
	}
}

void AAACharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	//ver 0.4.1 C
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::Run);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AAACharacterPlayer::StopRun);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::StartJump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeZoomAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::ChangeZoom);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::Look);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::StartFire);
	EnhancedInputComponent->BindAction(FireStopAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::StopFire);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::Reload);

}

void AAACharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAACharacterPlayer, PooledAmmoClass);
	DOREPLIFETIME(AAACharacterPlayer, SelectedAbility);
}

void AAACharacterPlayer::ChangeZoom()
{
	if (CurrentCharacterZoomType == ECharacterZoomType::ZoomOut)
	{
		//feat: 줌인 할 때 걷기로 초기화 ver 0.2.0 C
		StopRun();
		SetCharacterControl(ECharacterZoomType::ZoomIn);

		//SniferRifle UI
		if (WeaponData->Type == EWeaponType::SniperRifle)
		{
			FollowCamera->SetRelativeLocation(FVector(160.f, 30.f, 30.f));
			ShowScopeWidget();
		}
	}
	else if (CurrentCharacterZoomType == ECharacterZoomType::ZoomIn)
	{
		SetCharacterControl(ECharacterZoomType::ZoomOut);

		if (WeaponData->Type == EWeaponType::SniperRifle)
		{
			FollowCamera->SetRelativeLocation(FVector(0.0f, 45.0f, 85.0f));
			HideScopeWidget();
		}
	}
}

void AAACharacterPlayer::SetCharacterControl(ECharacterZoomType NewCharacterZoomType)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	UAACharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterZoomType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}
	
	CurrentCharacterZoomType = NewCharacterZoomType;
}

void AAACharacterPlayer::SetCharacterControlData(const UAACharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}

void AAACharacterPlayer::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AAACharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AAACharacterPlayer::Run()
{
	//feat: 달릴 때 줌인 되어있으면 리턴 ver 0.2.0 C
	// ver 0.5.1a
	// fix CanRun Logic
	if (CurrentCharacterZoomType == ECharacterZoomType::ZoomIn || bIsRun || bIsFiring || !bCanFire || GetCharacterMovement()->IsFalling())
	{
		return;
	}

	bIsRun = true;

	if (!HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed + (BaseMovementSpeed * 0.5f);
	}

	ServerRPCRun();
}

void AAACharacterPlayer::StopRun()
{
	bIsRun = false;

	if (!HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}

	ServerRPCStopRun();
}

bool AAACharacterPlayer::ServerRPCRun_Validate()
{
	return true;
}

void AAACharacterPlayer::ServerRPCRun_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Run"));

	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed + (BaseMovementSpeed * 0.5f);
}

bool AAACharacterPlayer::ServerRPCStopRun_Validate()
{
	return true;
}

void AAACharacterPlayer::ServerRPCStopRun_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Run"));

	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
}

void AAACharacterPlayer::StartJump()
{
	if (bIsRun)
	{
		UE_LOG(LogTemp, Log, TEXT("Start Jump While Running"));
		StopRun();
	}

	if (!bIsJump)
	{
		ACharacter::Jump();
		PlaySound(JumpSoundCue, GetActorLocation());
		bIsJump = true;
	}
}

void AAACharacterPlayer::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	PlaySound(LandSoundCue, GetActorLocation());
	bIsJump = false;
}

AAAWeaponAmmo* AAACharacterPlayer::GetPooledAmmo()
{
	if (AmmoPool.Num() == 0) Expand();

	return AmmoPool.Pop();
}

void AAACharacterPlayer::Expand()
{
	AmmoExpandSize = WeaponData->AmmoPoolExpandSize;

	for (int i = 0; i < AmmoExpandSize; i++)
	{
		// ver 0.3.1a
		// Zero Location에서 spawn시 즉시 return되어 GetPooledAmmo에서 Data를 못가져오는 Bug fix
		AAAWeaponAmmo* PoolableActor = GetWorld()->SpawnActor<AAAWeaponAmmo>(PooledAmmoClass, FVector(0.0f, 0.0f, -5000.f), FRotator().ZeroRotator);

		if (PoolableActor != nullptr)
		{
			// ver 0.8.2a
			// Apply Ammo Scale
			PoolableActor->SetActorScale3D(FVector(0.03f * AmmoScale, 0.03f * AmmoScale, 0.03f * AmmoScale));
			PoolableActor->SetDamage(AmmoDamage);
			PoolableActor->SetOwnerPlayer(this);
			PoolableActor->SetActive(false);
			AmmoPool.Add(PoolableActor);
		}
	}
	AmmoPoolSize += AmmoExpandSize;
}

void AAACharacterPlayer::ReturnAmmo(AAAWeaponAmmo* InReturnAmmoActor)
{
	if (InReturnAmmoActor)
	{
		AmmoPool.Push(InReturnAmmoActor);
		UE_LOG(LogTemp, Log, TEXT("Call Return Ammo"));
	}
}

void AAACharacterPlayer::ClearPool()
{
	TArray<AActor*> FoundAmmos;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAAWeaponAmmo::StaticClass(), FoundAmmos);

	for (AActor* ActiveAmmo : FoundAmmos)
	{
		AAAWeaponAmmo* PickCurrentAmmo = Cast<AAAWeaponAmmo>(ActiveAmmo);
		if (PickCurrentAmmo)
		{
			if (PickCurrentAmmo->AmmoType == EAmmoType::Rocket)
			{
				PickCurrentAmmo->Destroy();
			}
			else
			{
				PickCurrentAmmo->ReturnSelf();
			}
		}
	}

	for (int i = 0; i < AmmoPoolSize;)
	{
		AmmoPool.Pop()->Destroy();
		AmmoPoolSize--;
	}
}

void AAACharacterPlayer::Fire()
{
	if (bCanFire && CurrentAmmoSize > 0)
	{
		// Add Delay
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime >= NextFireTime)
		{
			FVector MuzzleLocation = Weapon->GetSocketLocation(FName("BarrelEndSocket"));

			// ver 0.4.2a
			// Fix Fire Direction
			FVector AimDirection = GetAdjustedAim();
			FVector FinalDirection;

			if (WeaponData->Type == EWeaponType::SniperRifle || WeaponData->Type == EWeaponType::Panzerfaust)
			{
				FinalDirection = AimDirection;
			}
			else
			{
				FinalDirection = GetMovementSpreadDirection(AimDirection);
			}
			

			ServerRPCFire(MuzzleLocation, FinalDirection);

			switch (WeaponData->Type)
			{
			case EWeaponType::Pistol:
				ApplyRecoil(AmmoDamage / 10);
				PlaySound(PSTFireSoundCue, MuzzleLocation);
				break;
			case EWeaponType::Rifle:
				ApplyRecoil(AmmoDamage / 20);
				PlaySound(ARFireSoundCue, MuzzleLocation);
				break;
			case EWeaponType::Shotgun:
				ApplyRecoil(AmmoDamage);
				PlaySound(SGFireSoundCue, MuzzleLocation);
				break;
			case EWeaponType::SniperRifle:
				ApplyRecoil(AmmoDamage / 10);
				PlaySound(SRFireSoundCue, MuzzleLocation);
				break;
			case EWeaponType::Panzerfaust:
				PlaySound(RPGFireSoundCue, MuzzleLocation);
				break;
			default:
				ApplyRecoil(AmmoDamage);
				PlaySound(PSTFireSoundCue, MuzzleLocation);
				break; 
			}

			if (WeaponData->Type != EWeaponType::Panzerfaust && WeaponData->Type != EWeaponType::Shotgun)
			{
				FTransform ShellTransform = Weapon->GetSocketTransform(FName("ShellSocket"));
				SpawnShell(ShellTransform);
			}
			NextFireTime = CurrentTime + RPM;

			RecoilStrength *= 1.1f;
		}
	}
	else
	{
		StopFire();
		PlaySound(MagEmptySoundCue, GetActorLocation());
	}
}

void AAACharacterPlayer::StartFire()
{
	//ver 0.4.1 C
	bIsFiring = true;

	if (bIsRun)
	{
		StopRun();
	}

	if (WeaponData)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AutomaticFire, this, &AAACharacterPlayer::Fire, RPM + 0.015f, true, 0.f);
	}
}

void AAACharacterPlayer::StopFire()
{
	//ver 0.4.1 C
	bIsFiring = false;

	RecoilStrength = 1.0f;

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_AutomaticFire);
}

FVector AAACharacterPlayer::GetAdjustedAim() const
{
	AAAPlayerController* PlayerController = Cast<AAAPlayerController>(GetController());
	FVector StartTrace;
	FRotator AimRotator;

	if (PlayerController)
	{
		PlayerController->GetPlayerViewPoint(StartTrace, AimRotator);
	}

	FVector EndTrace = StartTrace + (AimRotator.Vector() * 10000);
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, CollisionParams);

	if (HitResult.bBlockingHit)
	{
		return (HitResult.ImpactPoint - Weapon->GetSocketLocation(TEXT("BarrelEndSocket"))).GetSafeNormal();
	}


	FVector CameraStartLocation = FollowCamera->GetComponentLocation();
	FVector CameraEndLocation = CameraStartLocation + FollowCamera->GetForwardVector() * 1500.f;

	return (CameraEndLocation - Weapon->GetSocketLocation(TEXT("BarrelEndSocket"))).GetSafeNormal();
}

FVector AAACharacterPlayer::GetMovementSpreadDirection(const FVector& InAimDirection) const
{
	FVector MovementDirection = GetVelocity().GetSafeNormal2D();
	float Speed = GetVelocity().Size2D();

	float MaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	float SpeedRatio = FMath::Clamp(Speed / MaxSpeed, 0.0f, 1.0f);

	float SpreadAngle = SpeedRatio * 2.0f;
	FRotator SpreadRotator(0.0f, FMath::FRandRange(-SpreadAngle, SpreadAngle), 0.0f);

	FVector SpreadDirection = SpreadRotator.RotateVector(InAimDirection);

	return SpreadDirection;
}

bool AAACharacterPlayer::ServerRPCFire_Validate(const FVector& NewLocation, const FVector& NewDirection)
{
	// add validation logic later
	return bCanFire && (CurrentAmmoSize > 0);
}

void AAACharacterPlayer::ServerRPCFire_Implementation(const FVector& NewLocation, const FVector& NewDirection)
{
	// ver 0.3.1a
	// Panzerfaust Spawn Actor Per Fire
	if (WeaponData->Type == EWeaponType::Panzerfaust)
	{
		AAAWeaponAmmo* Rocket = GetWorld()->SpawnActor<AAAWeaponAmmo>(PooledAmmoClass, NewLocation, NewDirection.Rotation());
		if (Rocket)
		{
			Rocket->SetActorScale3D(FVector(1.f, 1.f * AmmoScale, 1.f * AmmoScale));
			Rocket->SetOwnerPlayer(this);
			Rocket->SetLifeSpan(4.0f);
			Rocket->SetActive(true);
			Rocket->Fire(NewDirection);
			MulticastRPCFire(Rocket, NewLocation, NewDirection);

			CurrentAmmoSize--;
		}
	}
	else if (WeaponData->Type == EWeaponType::Shotgun)
	{
		for (int i = 0; i < WeaponData->AmmoPoolExpandSize / 2; i++)
		{
			AAAWeaponAmmo* Bullet = GetPooledAmmo();

			if (Bullet)
			{
				FRotator BulletRotation = NewDirection.Rotation() + GetRandomRotator();
				Bullet->SetActorLocation(NewLocation);
				Bullet->SetActive(true);
				Bullet->Fire(BulletRotation.Vector());
				MulticastRPCFire(Bullet, NewLocation, BulletRotation.Vector());

				CurrentAmmoSize--;
			}
		}
	}
	else
	{
		AAAWeaponAmmo* Bullet = GetPooledAmmo();

		if (Bullet)
		{
			Bullet->SetActorLocation(NewLocation);
			Bullet->SetActive(true);
			Bullet->Fire(NewDirection);
			MulticastRPCFire(Bullet, NewLocation, NewDirection);

			CurrentAmmoSize--;
		}
	}

	if (CurrentAmmoSize == 0)
	{
		ServerRPCPlayReloadAnimation();

		if (CurrentCharacterZoomType == ECharacterZoomType::ZoomIn)
		{
			ChangeZoom();
		}
	}
}

void AAACharacterPlayer::MulticastRPCFire_Implementation(AAAWeaponAmmo* AmmoClass, const FVector& NewLocation, const FVector& NewDirection)
{
	if (AmmoClass)
	{
		AmmoClass->SetActorLocation(NewLocation);
		AmmoClass->Fire(NewDirection);
	}
}

void AAACharacterPlayer::SetPooledAmmoClass(UClass* NewAmmoClass)
{
	PooledAmmoClass = NewAmmoClass;
}

void AAACharacterPlayer::EquipAmmo(UClass* NewAmmoClass)
{
	if (NewAmmoClass)
	{
		if (!HasAuthority())
		{
			SetPooledAmmoClass(NewAmmoClass);
		}
		if (IsLocallyControlled())
		{
			ServerRPCSetPooledAmmoClass(NewAmmoClass);
		}
	}
	ClearPool();
}

bool AAACharacterPlayer::ServerRPCSetPooledAmmoClass_Validate(UClass* NewAmmoClass)
{
	return true;
}

void AAACharacterPlayer::ServerRPCSetPooledAmmoClass_Implementation(UClass* NewAmmoClass)
{
	SetPooledAmmoClass(NewAmmoClass);

	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				AAACharacterPlayer* OtherPlayer = Cast<AAACharacterPlayer>(PlayerController->GetPawn());
				if (OtherPlayer)
				{
					OtherPlayer->ClientRPCSetPooledAmmoClass(this, NewAmmoClass);
				}
			}
		}
	}
}

void AAACharacterPlayer::ClientRPCSetPooledAmmoClass_Implementation(AAACharacterPlayer* CharacterToPlay, UClass* NewAmmoClass)
{
	if (CharacterToPlay)
	{
		CharacterToPlay->SetPooledAmmoClass(NewAmmoClass);
	}
}

void AAACharacterPlayer::Reload()
{
	if (CurrentAmmoSize < MaxAmmoSize)
	{
		if (bIsRun)
		{
			StopRun();
		}

		if (IsLocallyControlled() && bCanFire)
		{
			ServerRPCPlayReloadAnimation();
		}

		if (CurrentCharacterZoomType == ECharacterZoomType::ZoomIn)
		{
			ChangeZoom();
		}
	}
}

bool AAACharacterPlayer::ServerRPCPlayReloadAnimation_Validate()
{
	return bCanFire;
}

void AAACharacterPlayer::ServerRPCPlayReloadAnimation_Implementation()
{
	MulticastRPCPlayReloadAnimation();
}

void AAACharacterPlayer::MulticastRPCPlayReloadAnimation_Implementation()
{
	PlayReloadAnimation();
}

FRotator AAACharacterPlayer::GetRandomRotator()
{
	float RandomPitch = FMath::RandRange(-1.0f, 1.0f);
	float RandomYaw = FMath::RandRange(-1.0f, 1.0f);
	float RandomRoll = FMath::RandRange(-1.0f, 1.0f);

	return FRotator(RandomPitch, RandomYaw, RandomRoll);
}

void AAACharacterPlayer::ApplyAbility()
{
	FAAAbilityStat AllAbility;
	for (int i = 0; i < SelectedAbilityArray.Num(); i++)
	{
		AllAbility = AllAbility + SelectedAbilityArray[i];
	}

	if (!HasAuthority())
	{
		SetAllAbility(AllAbility);
		SetAbilityInController(AllAbility);
	}
	if (IsLocallyControlled())
	{
		ServerRPCApplyAbility(AllAbility);
	}
}

void AAACharacterPlayer::ServerRPCApplyAbility_Implementation(const FAAAbilityStat& NewAbilityStat)
{
	
	SetAllAbility(NewAbilityStat);
	SetAbilityInController(NewAbilityStat);

	ClearPool();
	//UE_LOG(LogAACharacter, Error, TEXT("Server"));

	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				AAACharacterPlayer* OtherPlayer = Cast<AAACharacterPlayer>(PlayerController->GetPawn());
				if (OtherPlayer)
				{
					OtherPlayer->ClientRPCApplyAbility(this, NewAbilityStat);
				}
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("ServerRPCApplyAbility: New MaxHp = %f"), Stat->GetMaxHp());
}

void AAACharacterPlayer::ClientRPCApplyAbility_Implementation(AAACharacterPlayer* CharacterToPlay, const FAAAbilityStat& NewAbilityStat)
{
	if (CharacterToPlay)
	{
		CharacterToPlay->SetAllAbility(NewAbilityStat);
	}
}

void AAACharacterPlayer::SetAbility(const FAAAbilityStat& InAddAbility)
{
	SelectedAbility = InAddAbility;
	SelectedAbilityArray.Add(SelectedAbility);
}

void AAACharacterPlayer::SetAllAbility(const FAAAbilityStat& NewAbilityStat)
{
		Stat->SetNewMaxHp(Stat->GetBaseStat().MaxHp * NewAbilityStat.MaxHp);

		BaseMovementSpeed = Stat->GetTotalStat().MovementSpeed * NewAbilityStat.MovementSpeed;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

		RPM = FMath::Clamp(WeaponData->WeaponStat.RPM + (WeaponData->WeaponStat.RPM * NewAbilityStat.RPM), 0.05f, 10.f);
		AmmoDamage = FMath::Clamp(WeaponData->AmmoDamage * NewAbilityStat.Damage, 1, 999);
		AmmoSpeed = WeaponData->AmmoSpeed * NewAbilityStat.AmmoSpeed;
		AmmoScale = NewAbilityStat.AmmoScale;
		Acceleration = NewAbilityStat.Acceleration;

		MaxAmmoSize = (int32)(WeaponData->AmmoPoolExpandSize * NewAbilityStat.AmmoSize);
		CurrentAmmoSize = MaxAmmoSize;

		ReloadSpeed = NewAbilityStat.ReloadSpeed;
		SplashRound = NewAbilityStat.SplashRound;

		bBloodDrain = (bool)NewAbilityStat.BloodDrain;

		Magnification = NewAbilityStat.Magnification;
}

void AAACharacterPlayer::SetAbilityInController(const FAAAbilityStat& NewAbilityStat)
{
	if (IsLocallyControlled())
	{
		AAAPlayerController* testController = Cast<AAAPlayerController>(GetController());
		if (testController)
		{
			UAAGameInstance* PC = Cast<UAAGameInstance>(testController->GetGameInstance());
			PC->SetPlayerStat(NewAbilityStat);
			PC->SetAmmoClass(PooledAmmoClass);
		}
	}
}

void AAACharacterPlayer::SetAbilityBeginPlay()
{
	if (IsLocallyControlled())
	{
		AAAPlayerController* testController = Cast<AAAPlayerController>(GetController());
		if (testController)
		{
			UAAGameInstance* PC = Cast<UAAGameInstance>(testController->GetGameInstance());
			if (PC)
			{
				SetAbility(testController->SendGameInstance());
				EquipAmmo(PC->GetAmmoClass());
				PC->SetAmmoClass(PC->GetAmmoClass());
				ApplyAbility();
			}
		}
	}
}

void AAACharacterPlayer::SetPlayerStopFire()
{
	bCanFire = false;
}

void AAACharacterPlayer::ShowScopeWidget()
{
	if (ScopeWidgetClass && !ScopeWidgetInstance)
	{
		ScopeWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), ScopeWidgetClass);
		if (ScopeWidgetInstance)
		{
			ScopeWidgetInstance->AddToViewport(-1);
		}
	}
}

void AAACharacterPlayer::HideScopeWidget()
{
	if (ScopeWidgetInstance)
	{
		ScopeWidgetInstance->RemoveFromParent();
		ScopeWidgetInstance = nullptr;
	}
}
