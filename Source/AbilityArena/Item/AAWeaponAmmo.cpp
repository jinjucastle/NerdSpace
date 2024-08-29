// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AAWeaponAmmo.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/AACharacterPlayer.h"
#include "Player/AAPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"

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
	if (HasAuthority())
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

					if (HitEffect)
					{
						MulticastRPCPlayEffect(HitEffect, GetActorLocation());
					}

					if (OwnerCharacter->GetCanBloodDrain())
					{
						OwnerCharacter->BloodDrain(AppliedDamage);
					}
				}
			}
			else
			{
				if (AmmoEffect)
				{
					MulticastRPCPlayEffect(AmmoEffect, GetActorLocation());

					PlaySoundCue();
				}

				//Impulse Physics Actor
				if (UPrimitiveComponent* OtherCompPrimitive = Cast<UPrimitiveComponent>(OtherComp))
				{
					FVector ImpulseDirection = (OtherActor->GetActorLocation() - OwnerLocation).GetSafeNormal();
					FVector Impulse = ImpulseDirection * Damage * 10;

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
		if (OtherActor == Owner)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ignore"));
			return;
		}
	}

	if (HasAuthority())
	{
		if (AmmoType != EAmmoType::Rocket)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ignore"));
			return;
		}

		if (!Other->IsA(AAAWeaponAmmo::StaticClass()))
		{
			ApplySplashDamage();
			if (AmmoEffect)
			{
				MulticastRPCPlayEffect(AmmoEffect, GetActorLocation());

				PlaySoundCue();
			}
			Destroy();
			UE_LOG(LogTemp, Log, TEXT("Destroy"));
		}

		UE_LOG(LogTemp, Log, TEXT("Server Hit: %s"), *Hit.GetActor()->GetName());
	}

	UE_LOG(LogTemp, Log, TEXT("All Hit: %s"), *Hit.GetActor()->GetName());
}

void AAAWeaponAmmo::Fire(const FVector& FireDirection) const
{
	AmmoMovement->Velocity = FireDirection * AmmoMovement->InitialSpeed;
	if (AmmoType == EAmmoType::Rocket)
	{
		UE_LOG(LogTemp, Log, TEXT("Fire called with direction: %s, AmmoSpeed = %f"), *FireDirection.ToString(), AmmoMovement->InitialSpeed);
	}
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
	float DamageInnerRadius = 150.f * SplashRound;
	float DamageOuterRadius = 300.0f * SplashRound;
	float DamageFalloff = 1.0f;

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

	TArray<FHitResult> OutHits;

	FVector Start = GetActorLocation();
	FVector End = Start;

	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(DamageOuterRadius);

	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, Start, End, FQuat::Identity, ECC_WorldStatic, MyColSphere);

	if (isHit)
	{
		for (auto& Hit : OutHits)
		{
			UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>((Hit.GetActor())->GetRootComponent());

			if (MeshComp)
			{
				if (MeshComp->IsSimulatingPhysics())
				{
					MeshComp->AddRadialImpulse(GetActorLocation(), DamageOuterRadius, Damage * 10, ERadialImpulseFalloff::RIF_Linear, true);
					UE_LOG(LogTemp, Log, TEXT("Hit Mesh: %s"), *Hit.GetActor()->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("No physics simulation for: %s"), *(Hit.GetActor()->GetName()));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No mesh component simulation for: %s"), *(Hit.GetActor()->GetName()));
			}
		}
	}
}

void AAAWeaponAmmo::PlaySoundCue()
{
	if (AmmoSoundCue)
	{
		MulticastRPCPlaySound(AmmoSoundCue, GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AmmoSoundCue is null"));
	}
}

void AAAWeaponAmmo::MulticastRPCPlayEffect_Implementation(UNiagaraSystem* InEffect, FVector Location)
{
	if (AmmoType == EAmmoType::Rocket)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), InEffect, Location, FRotator::ZeroRotator, FVector(SplashRound));
	}
	else
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), InEffect, Location);
	}
}

void AAAWeaponAmmo::MulticastRPCPlaySound_Implementation(USoundCue* SoundCue, FVector Location)
{
	if (SoundCue)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, AmmoSoundCue, GetActorLocation());
		UE_LOG(LogTemp, Error, TEXT("Play SoundCue"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Multicast PlaySound Cue is null"));
	}
}
