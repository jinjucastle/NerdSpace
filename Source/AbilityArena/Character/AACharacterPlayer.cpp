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
#include "Item/AAWeaponItemData.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "CharacterStat/AACharacterPlayerState.h"
#include "Kismet/GameplayStatics.h"

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
}

void AAACharacterPlayer::ChangeZoom()
{
	if (CurrentCharacterZoomType == ECharacterZoomType::ZoomOut)
	{
		//feat: 줌인 할 때 걷기로 초기화 ver 0.2.0 C
		StopRun();

		SetCharacterControl(ECharacterZoomType::ZoomIn);
	}
	else if (CurrentCharacterZoomType == ECharacterZoomType::ZoomIn)
	{
		SetCharacterControl(ECharacterZoomType::ZoomOut);
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
		float MovementSpeed = Stat->GetBaseStat().MovementSpeed + Stat->GetWeaponStat().MovementSpeed;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed + (MovementSpeed * 0.75f);
	}

	ServerRPCRun();
}

void AAACharacterPlayer::StopRun()
{
	bIsRun = false;

	if (!HasAuthority())
	{
		float MovementSpeed = Stat->GetBaseStat().MovementSpeed + Stat->GetWeaponStat().MovementSpeed;
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
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

	float MovementSpeed = Stat->GetBaseStat().MovementSpeed + Stat->GetWeaponStat().MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed + (MovementSpeed * 0.75f);
}

bool AAACharacterPlayer::ServerRPCStopRun_Validate()
{
	return true;
}

void AAACharacterPlayer::ServerRPCStopRun_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Run"));

	float MovementSpeed = Stat->GetBaseStat().MovementSpeed + Stat->GetWeaponStat().MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AAACharacterPlayer::StartJump()
{
	if (bIsRun)
	{
		UE_LOG(LogTemp, Log, TEXT("Start Jump While Running"));
		StopRun();
	}

	ACharacter::Jump();
}

AAAWeaponAmmo* AAACharacterPlayer::GetPooledAmmo()
{
	if (AmmoPool.IsEmpty()) Expand();
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
			PoolableActor->SetActive(false);
			PoolableActor->SetOwnerPlayer(this);
			AmmoPool.Push(PoolableActor);
			AmmoPoolSize++;
		}
	}
}

void AAACharacterPlayer::ReturnAmmo(AAAWeaponAmmo* InReturnAmmoActor)
{
	AmmoPool.Push(InReturnAmmoActor);
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
	//check(AmmoPool.IsEmpty() && AmmoPoolSize == 0);
}

void AAACharacterPlayer::Fire()
{

	if (bCanFire && CurrentAmmoSize > 0)
	{
		FVector MuzzleLocation = Weapon->GetSocketLocation(FName("BarrelEndSocket"));
		FRotator MuzzleRotation = Weapon->GetSocketRotation(FName("BarrelEndSocket"));

		ServerRPCFire(MuzzleLocation, MuzzleRotation);

	}
	else
	{
		StopFire();
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
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AutomaticFire, this, &AAACharacterPlayer::Fire, WeaponData->WeaponStat.RPM + 0.01f, true, 0.f);
	}
}

void AAACharacterPlayer::StopFire()
{
	//ver 0.4.1 C
	bIsFiring = false;

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_AutomaticFire);
}

bool AAACharacterPlayer::ServerRPCFire_Validate(const FVector& NewLocation, const FRotator& NewRotation)
{
	// add validation logic later
	return true;
}

void AAACharacterPlayer::ServerRPCFire_Implementation(const FVector& NewLocation, const FRotator& NewRotation)
{
	// ver 0.4.2a
	// Fix Fire Direction
	FVector CameraStartLocation = FollowCamera->GetComponentLocation();
	FVector CameraEndLocation = CameraStartLocation + FollowCamera->GetForwardVector() * 5000.f;
	FVector Direction = CameraEndLocation - CameraStartLocation;

	FRotator FireDirection = Direction.Rotation();

	// ver 0.3.1a
	// Panzerfaust Spawn Actor Per Fire
	if (WeaponData->Type == EWeaponType::Panzerfaust)
	{
		AAAWeaponAmmo* Rocket = GetWorld()->SpawnActor<AAAWeaponAmmo>(PooledAmmoClass, NewLocation, FireDirection);
		Rocket->SetOwnerPlayer(this);
		Rocket->SetLifeSpan(2.0f);
		Rocket->SetActive(true);
		if (Rocket)
		{
			Rocket->Fire();
			CurrentAmmoSize--;
		}
	}
	else if (WeaponData->Type == EWeaponType::Shotgun)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime >= NextFireTime)
		{
			for (int i = 0; i < WeaponData->AmmoPoolExpandSize / 2; i++)
			{
				AAAWeaponAmmo* Bullet = GetPooledAmmo();

				if (Bullet != nullptr)
				{
					FRotator BulletRotation = FireDirection + GetRandomRotator();
					//Bullet->SetReplicatedRotation(BulletRotation);
					Bullet->SetActorLocationAndRotation(NewLocation, BulletRotation);
					Bullet->SetActive(true);
					Bullet->Fire();

					CurrentAmmoSize--;

					NextFireTime = CurrentTime + WeaponData->WeaponStat.RPM;
				}
			}
		}
	}
	else
	{
		// Add Delay
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime >= NextFireTime)
		{
			AAAWeaponAmmo* Bullet = GetPooledAmmo();

			if (Bullet != nullptr)
			{
				//Bullet->SetReplicatedRotation(NewRotation);
				Bullet->SetActorLocationAndRotation(NewLocation, FireDirection);
				Bullet->SetActive(true);
				Bullet->Fire();

				CurrentAmmoSize--;

				NextFireTime = CurrentTime + WeaponData->WeaponStat.RPM;
			}
		}
	}

	if (CurrentAmmoSize == 0)
	{
		Reload();
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
		SetPooledAmmoClass(NewAmmoClass);
	
		if (!HasAuthority())
		{
			ServerRPCSetPooledAmmoClass(NewAmmoClass);
		}
	}
	ClearPool();
}

void AAACharacterPlayer::OnRep_PooledAmmoClass()
{
	EquipAmmo(PooledAmmoClass);
}

bool AAACharacterPlayer::ServerRPCSetPooledAmmoClass_Validate(UClass* NewAmmoClass)
{
	return true;
}

void AAACharacterPlayer::ServerRPCSetPooledAmmoClass_Implementation(UClass* NewAmmoClass)
{
	PooledAmmoClass = NewAmmoClass;

	OnRep_PooledAmmoClass();
}

void AAACharacterPlayer::Reload()
{
	if (bIsRun)
	{
		StopRun();
	}

	if (HasAuthority())
	{
		PlayReloadAnimation();
		MulticastRPCPlayReloadAnimation();
	}
}


FRotator AAACharacterPlayer::GetRandomRotator()
{
	float RandomPitch = FMath::RandRange(-1.0f, 1.0f);
	float RandomYaw = FMath::RandRange(-1.0f, 1.0f);
	float RandomRoll = FMath::RandRange(-1.0f, 1.0f); 

	return FRotator(RandomPitch, RandomYaw, RandomRoll);
}


void AAACharacterPlayer::MulticastRPCPlayReloadAnimation_Implementation()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("MulticastRPCPlayerReloadAnimation"));
		PlayReloadAnimation();
	}
}
