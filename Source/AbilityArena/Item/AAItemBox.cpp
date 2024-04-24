// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/AAItemBox.h"
#include "Components/BoXComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Interface/AACharacterItemInterface.h"

#include "Character/AACharacterPlayer.h"


// Sets default values
AAAItemBox::AAAItemBox()
{
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

    RootComponent = Trigger;
    Mesh->SetupAttachment(Trigger);
    Effect->SetupAttachment(Trigger);

    //?
    //test setting
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    //Trigger->SetCollisionProfileName(TEXT("AATrigger"));
    Trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &AAAItemBox::OnOverlapBegin);


    static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/Model/Item/Environment/Props/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1'"));
    if (BoxMeshRef.Object)
    {
        Mesh->SetStaticMesh(BoxMeshRef.Object);
    }
    Mesh->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));
    Mesh->SetCollisionProfileName(TEXT("NoCollsion"));

    static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/Effect/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh'"));
    if (EffectRef.Object)
    {
        Effect->SetTemplate(EffectRef.Object);
        Effect->bAutoActivate = false;
    }
}

void AAAItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
    //?
    if (!OtherActor->IsA(AAACharacterPlayer::StaticClass()))    return;
 
    //²Î
    if (nullptr == Item)
    {
        Destroy();
        return;
    }

    IAACharacterItemInterface* OverlappingPawn = Cast<IAACharacterItemInterface>(OtherActor);
    if (OverlappingPawn)
    {
        OverlappingPawn->TakeItem(Item);
    }

    Effect->Activate(true);
    Mesh->SetHiddenInGame(true);
    SetActorEnableCollision(false);
    Effect->OnSystemFinished.AddDynamic(this, &AAAItemBox::OnEffectFinished);

}

void AAAItemBox::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
    Destroy();
}


