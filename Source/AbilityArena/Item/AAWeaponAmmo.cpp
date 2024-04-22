// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AAWeaponAmmo.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/AACharacterPlayer.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AAAWeaponAmmo::AAAWeaponAmmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	AmmoMesh->SetupAttachment(Root);
	AmmoMesh->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	//test setting
	AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	AmmoMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("AmmoMovement"));
	AmmoMovement->SetUpdatedComponent(Root);
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
	if (bIsActive)
	{
		if (AmmoType == EAmmoType::Rocket)
		{
			FVector NewVelocity = AmmoMovement->Velocity * 1.03f;
			AmmoMovement->Velocity = NewVelocity;
		}
	}
}

void AAAWeaponAmmo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAAWeaponAmmo, ReplicatedRotation);
}

void AAAWeaponAmmo::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!OtherActor->IsA(AAAWeaponAmmo::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("Notify Begine Overlap")));
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

void AAAWeaponAmmo::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (!OtherActor->IsA(AAAWeaponAmmo::StaticClass()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Notify End Overlap")));
	}
}

void AAAWeaponAmmo::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	//ver 0.2.1JH Change Log 


	if (Other)
	{
		USkeletalMeshComponent* TestCom = Other->FindComponentByClass<USkeletalMeshComponent>();
		if (TestCom)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Notify Hit")));
			FName BoonNameText = Hit.BoneName;
			UE_LOG(LogTemp, Warning, TEXT("TestConllision: %s"), *BoonNameText.ToString());
		}

		// ver 0.3.1a
		// Rocket Ammo Destroy
		if (AmmoType == EAmmoType::Rocket)
		{
			Destroy();
		}
		else
		{
			ReturnSelf();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("this is not Actor"));
	}
}

void AAAWeaponAmmo::Fire() const
{
	AmmoMovement->SetVelocityInLocalSpace(FVector::ForwardVector * AmmoMovement->InitialSpeed);
}

void AAAWeaponAmmo::SetOwnerPlayer(AAACharacterPlayer* InPlayer)
{
	if (AmmoType != EAmmoType::Rocket)
	{
		Owner = InPlayer;
	}
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
	SetActive(false);
}

void AAAWeaponAmmo::SetActive(bool InIsActive)
{
	bIsActive = InIsActive;
	SetActorHiddenInGame(!bIsActive);
	SetActorEnableCollision(bIsActive);

	if (bIsActive && AmmoType != EAmmoType::Rocket)
	{
		GetWorld()->GetTimerManager().SetTimer(ActiveHandle, this, &AAAWeaponAmmo::ReturnSelf, 2.0f, false);
	}
}

void AAAWeaponAmmo::OnRep_RotationUpdated()
{
	SetActorRotation(ReplicatedRotation);
}
