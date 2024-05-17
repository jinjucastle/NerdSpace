// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AAWeaponAmmo.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/AACharacterPlayer.h"

// Sets default values
AAAWeaponAmmo::AAAWeaponAmmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	AmmoMesh->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	AmmoMesh->SetNotifyRigidBodyCollision(true);

	RootComponent = AmmoMesh;

	AmmoMesh->OnComponentBeginOverlap.AddDynamic(this, &AAAWeaponAmmo::OnOverlapBegin);

	AmmoMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("AmmoMovement"));
	AmmoMovement->SetUpdatedComponent(AmmoMesh);
	AmmoMovement->bRotationFollowsVelocity = true;
	AmmoMovement->bShouldBounce = false;

	AmmoMovement->ProjectileGravityScale = 0.0f;

	AmmoType = EAmmoType::Normal;
}

// Called when the game starts or when spawned
void AAAWeaponAmmo::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAAWeaponAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ver 0.1.3a
	// Rocket Acceleration
	if (Owner)
	{
		if (bIsActive)
		{
			if (AmmoType == EAmmoType::Rocket)
			{
				FVector NewVelocity = AmmoMovement->Velocity * Owner->GetAcceleration();
				AmmoMovement->Velocity = NewVelocity;
			}
		}
	}
}

void AAAWeaponAmmo::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
}

void AAAWeaponAmmo::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->StaticClass() == Owner->StaticClass())
	{
		UE_LOG(LogTemp, Warning, TEXT("Error"));
	}

	if (!OtherActor->IsA(AAAWeaponAmmo::StaticClass()))
	{
		if (OtherActor->GetLocalRole() == ROLE_Authority)
		{
			USkeletalMeshComponent* TestCom = OtherActor->FindComponentByClass<USkeletalMeshComponent>();
			if (TestCom)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Overlap Begin")));
				FName BoneName = SweepResult.BoneName;
				UE_LOG(LogTemp, Warning, TEXT("TestConllision: %s"), *BoneName.ToString());
			}
		}

		if (AmmoType == EAmmoType::Rocket)
		{
			Destroy();
			UE_LOG(LogTemp, Log, TEXT("Destroy"));
		}
		else
		{
			ReturnSelf();
			UE_LOG(LogTemp, Log, TEXT("Return"));
		}
	}
}

void AAAWeaponAmmo::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Other->StaticClass() == Owner->StaticClass())
	{
		UE_LOG(LogTemp, Warning, TEXT("Error"));
	}

	if (!Other->IsA(AAAWeaponAmmo::StaticClass()))
	{
		FString RoleName;
		if (Other->GetLocalRole() == ROLE_Authority)
		{
			RoleName = TEXT("Server (Autonomous Proxy)");
		}
		else if (Other->GetLocalRole() == ROLE_AutonomousProxy)
		{
			RoleName = TEXT("Client (Autonomous Proxy)");
		}
		else if (Other->GetLocalRole() == ROLE_SimulatedProxy)
		{
			RoleName = TEXT("Client (Simulated Proxy)");
		}
		else
		{
			RoleName = TEXT("Unknown Role");
		}

		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Hit : %s | Role : %s"), *Other->GetName(), *RoleName));
		if (AmmoType == EAmmoType::Rocket)
		{
			Destroy();
		}
		else
		{
			ReturnSelf();
		}
	}
}

void AAAWeaponAmmo::Fire() const
{
	if (AmmoMovement)
	{
		FVector FireDirection = GetActorForwardVector();
		AmmoMovement->Velocity = FireDirection * AmmoMovement->InitialSpeed;
	}
}

void AAAWeaponAmmo::SetOwnerPlayer(AAACharacterPlayer* InPlayer)
{
	Owner = InPlayer;
	Damage = Owner->GetAmmoDamage();
	AmmoMovement->InitialSpeed = Owner->GetAmmoSpeed();
	AmmoMovement->MaxSpeed = Owner->GetAmmoSpeed() * 10;
}

void AAAWeaponAmmo::ReturnSelf()
{
	if (Owner == nullptr)
	{
		Destroy();
		return;
	}
	if (!bIsActive) return;
	Owner->ReturnAmmo(this);
}

void AAAWeaponAmmo::SetActive(bool InIsActive)
{
	bIsActive = InIsActive;
	SetActorHiddenInGame(!bIsActive);
	SetActorEnableCollision(bIsActive);
	SetActorTickEnabled(bIsActive);

	if (bIsActive && AmmoType != EAmmoType::Rocket)
	{
		GetWorld()->GetTimerManager().SetTimer(ActiveHandle, this, &AAAWeaponAmmo::ReturnSelf, 4.0f, false);
	}
}