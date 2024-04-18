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
	//SetReplicates(true);
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

	//ver 0.2.1JH shoot lifeSpan
	InitialLifeSpan = 3.0f;
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

}

void AAAWeaponAmmo::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("Notify Begine Overlap")));
	ReturnSelf();
}

void AAAWeaponAmmo::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Notify End Overlap")));
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
			//ReturnSelf();
		}
		this->Destroy();
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

void AAAWeaponAmmo::TestFire(const FVector& ShootDirection)
{
	AmmoMovement->InitialSpeed = 3000.0f;
	AmmoMovement->Velocity = ShootDirection * AmmoMovement->InitialSpeed;
	AmmoMesh->SetPhysicsLinearVelocity(AmmoMovement->Velocity);
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
	if (Owner == nullptr) return;
	if (!bIsActive) return;
	Owner->ReturnAmmo(this);
	SetActive(false);
}

void AAAWeaponAmmo::SetActive(bool InIsActive)
{
	bIsActive = InIsActive;
	SetActorHiddenInGame(!bIsActive);

	if (bIsActive)
	{
		GetWorld()->GetTimerManager().SetTimer(ActiveHandle, this, &AAAWeaponAmmo::ReturnSelf, 1.0f, false);
	}
}
