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

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionRunRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Action/IA_Run.IA_Run'"));
	if (nullptr != InputActionRunRef.Object)
	{
		RunAction = InputActionRunRef.Object;
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

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeZoomAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::ChangeZoom);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::Look);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::Fire);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AAACharacterPlayer::Run);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AAACharacterPlayer::StopRun);
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
	if (CurrentCharacterZoomType == ECharacterZoomType::ZoomIn)
	{
		return;
	}

	float MovementSpeed = Stat->GetBaseStat().MovementSpeed + Stat->GetWeaponStat().MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed + (MovementSpeed * 0.75f);
}

void AAACharacterPlayer::StopRun()
{
	float MovementSpeed = Stat->GetBaseStat().MovementSpeed + Stat->GetWeaponStat().MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
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
		AAAWeaponAmmo* PoolableActor = GetWorld()->SpawnActor<AAAWeaponAmmo>(PooledAmmoClass, FVector().ZeroVector, FRotator().ZeroRotator);
		PoolableActor->SetActive(false);
		PoolableActor->SetOwnerPlayer(this);
		AmmoPool.Push(PoolableActor);
	}
	AmmoPoolSize += AmmoExpandSize;
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
			PickCurrentAmmo->ReturnSelf();
		}
	}

	for (int i = 0; i < AmmoPoolSize;)
	{
		AmmoPool.Pop()->Destroy();
		AmmoPoolSize--;
	}
	check(AmmoPool.IsEmpty() && AmmoPoolSize == 0);
}

void AAACharacterPlayer::Fire()
{
	FVector CameraStartLocation = FollowCamera->GetComponentLocation();
	FVector CameraEndLocation = CameraStartLocation + FollowCamera->GetForwardVector() * 5000.0f;

	FVector MuzzleLocation = Weapon->GetSocketLocation(FName("BarrelEndSocket"));
	FRotator MuzzleRotation = Weapon->GetSocketRotation(FName("BarrelEndSocket"));

	if (!HasAuthority())
	{
		ServerRPCFire(MuzzleLocation, MuzzleRotation);
	}
	else
	{
		ClientRPCFire(MuzzleLocation, MuzzleRotation);
	}
}

bool AAACharacterPlayer::ServerRPCFire_Validate(const FVector& NewLocation, const FRotator& NewRotation)
{
	// add validation logic later
	return true;
}

void AAACharacterPlayer::ServerRPCFire_Implementation(const FVector& NewLocation, const FRotator& NewRotation)
{
	//ver 0.2.1B Change way of shooting 
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		AAAWeaponAmmo* PoolableActor = GetWorld()->SpawnActor<AAAWeaponAmmo>(PooledAmmoClass, NewLocation, NewRotation);
		if (PoolableActor)
		{
			FVector LaunchDirection = NewRotation.Vector();
			PoolableActor->TestFire(LaunchDirection);
		}
	}
	/*AAAWeaponAmmo* Bullet = GetPooledAmmo();

	Bullet->SetActorLocation(NewLocation);
	Bullet->SetActorRotation(NewRotation);

	Bullet->SetActive(true);
	Bullet->Fire();
	*/
}

void AAACharacterPlayer::ClientRPCFire_Implementation(const FVector& NewLocation, const FRotator& NewRotation)
{
	//ver 0.2.1B Change way of shooting 
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		AAAWeaponAmmo* PoolableActor = GetWorld()->SpawnActor<AAAWeaponAmmo>(PooledAmmoClass, NewLocation, NewRotation);
		if (PoolableActor)
		{
			FVector LaunchDirection = NewRotation.Vector();
			PoolableActor->TestFire(LaunchDirection);
		}
	}
	/*AAAWeaponAmmo* Bullet = GetPooledAmmo();

	Bullet->SetActorLocation(NewLocation);
	Bullet->SetActorRotation(NewRotation);

	Bullet->SetActive(true);
	Bullet->Fire();
	*/
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
	UE_LOG(LogTemp, Warning, TEXT("Call OnRep_PooledAmmoClass"));
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