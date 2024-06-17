// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AAWeaponAmmo.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/AACharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AAAWeaponAmmo::AAAWeaponAmmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));

	RootComponent = AmmoMesh;

	AmmoMesh->OnComponentBeginOverlap.AddDynamic(this, &AAAWeaponAmmo::OnOverlapBegin);

	AmmoMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("AmmoMovement"));
	AmmoMovement->SetUpdatedComponent(AmmoMesh);
	AmmoMovement->bRotationFollowsVelocity = true;
	AmmoMovement->bShouldBounce = false;
	AmmoMovement->StopMovementImmediately();

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

void AAAWeaponAmmo::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AAACharacterPlayer* Other = Cast<AAACharacterPlayer>(OtherActor))
	{
		if (Other == Owner || AmmoType != EAmmoType::Normal)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ignore"));
			return;
		}
	}

	if (!OtherActor->IsA(AAAWeaponAmmo::StaticClass()))
	{
		if (HasAuthority())
		{
			//Character Damage
			if (OtherActor->IsA(AAACharacterPlayer::StaticClass()))
			{
				AAACharacterPlayer* OwnerCharacter = Cast<AAACharacterPlayer>(Owner);
				FName BoneName = SweepResult.BoneName;
				int32 AppliedDamage;
				UE_LOG(LogTemp, Warning, TEXT("TestConllision: %s"), *BoneName.ToString());

				if (!BoneName.IsNone())
				{
					if (BoneName.ToString().Contains(TEXT("DEF-spine"), ESearchCase::CaseSensitive))
					{
						if (BoneName.ToString().Contains(TEXT("_006"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
						{
							AppliedDamage = (int32)(Damage * 1.5f);
						}
						else
						{
							AppliedDamage = (int32)(Damage);
						}
					}
					else
					{
						AppliedDamage = (int32)(Damage * 0.5f);
					}

					UGameplayStatics::ApplyDamage(OtherActor, (float)AppliedDamage, Owner->GetController(), this, UDamageType::StaticClass());

					if (OwnerCharacter->GetCanBloodDrain())
					{
						OwnerCharacter->BloodDrain(AppliedDamage);
					}
				}
			}
			else
			{
				if (NormalAmmoEffect)
				{
					FVector SpawnLocation = GetActorLocation();

					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NormalAmmoEffect, SpawnLocation);

					UE_LOG(LogTemp, Log, TEXT("Niagara Effect Spawned at Location: %s"), *SpawnLocation.ToString());

				}

				//Impulse Physics Actor
				if (UPrimitiveComponent* OtherCompPrimitive = Cast<UPrimitiveComponent>(OtherComp))
				{
					FVector ImpulseDirection = (OtherActor->GetActorLocation() - OwnerLocation).GetSafeNormal();
					FVector Impulse = ImpulseDirection * Damage * 50;

					//OtherCompPrimitive->AddImpulse(Impulse, NAME_None, true);
					MulticastRPCApplyImpulse(OtherCompPrimitive, Impulse);
					UE_LOG(LogTemp, Log, TEXT("Impulse"));
				}
			}
			ReturnSelf();
			UE_LOG(LogTemp, Log, TEXT("Return"));
		}
	}
}

void AAAWeaponAmmo::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (AAACharacterPlayer* OtherActor = Cast<AAACharacterPlayer>(Other))
	{
		if (OtherActor->GetController() == Owner->GetController())
		{
			UE_LOG(LogTemp, Warning, TEXT("Ignore"));
			return;
		}
	}

	if (AmmoType != EAmmoType::Rocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ignore"));
		return;
	}

	if (!Other->IsA(AAAWeaponAmmo::StaticClass()))
	{
		if (HasAuthority())
		{
			ApplySplashDamage();

			Destroy();

			if (RocketAmmoEffect)
			{
				FVector SpawnLocation = GetActorLocation();

				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RocketAmmoEffect, SpawnLocation);
			}
			UE_LOG(LogTemp, Log, TEXT("Destroy"));
		}
	}
}

void AAAWeaponAmmo::Fire(const FVector& FireDirection) const
{
	AmmoMovement->Velocity = FireDirection * AmmoMovement->InitialSpeed;
}

void AAAWeaponAmmo::MulticastRPCApplyImpulse_Implementation(UPrimitiveComponent* OverlappedComp, const FVector& Impulse)
{
	if (OverlappedComp && OverlappedComp->IsSimulatingPhysics())
	{
		OverlappedComp->AddImpulse(Impulse, NAME_None, true);
	}
}

void AAAWeaponAmmo::SetOwnerPlayer(AAACharacterPlayer* InPlayer)
{
	Owner = InPlayer;
	Damage = Owner->GetAmmoDamage();
	SplashRound = Owner->GetSplashRound();
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

	SetActive(false);
	AmmoMovement->StopMovementImmediately();
	SetActorLocation(Owner->GetActorLocation());
	Owner->ReturnAmmo(this);
}

void AAAWeaponAmmo::SetActive(bool InIsActive)
{
	bIsActive = InIsActive;
	SetActorHiddenInGame(!bIsActive);
	SetActorEnableCollision(bIsActive);

	if (bIsActive)
	{
		OwnerLocation = Owner->GetActorLocation();
	}

	if (bIsActive && AmmoType != EAmmoType::Rocket)
	{
		GetWorld()->GetTimerManager().SetTimer(ActiveHandle, this, &AAAWeaponAmmo::ReturnSelf, 4.0f, false);
	}
}

void AAAWeaponAmmo::ApplySplashDamage()
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	float BaseDamage = Damage;
	float MinimumDamage = Damage / 10;
	float DamageInnerRadius = 100.f * SplashRound;
	float DamageOuterRadius = 300.0f * SplashRound;
	float DamageFalloff = 5.0f;

	UGameplayStatics::ApplyRadialDamageWithFalloff(
		this,
		BaseDamage,
		MinimumDamage,
		GetActorLocation(),
		DamageInnerRadius,
		DamageOuterRadius,
		DamageFalloff,
		UDamageType::StaticClass(),
		IgnoredActors,
		this,
		Owner->GetController(),
		ECC_Visibility
	);
}
