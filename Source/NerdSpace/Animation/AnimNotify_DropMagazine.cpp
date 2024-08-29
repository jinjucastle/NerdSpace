// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_DropMagazine.h"
#include "Character/AACharacterPlayer.h"
#include "Item/AAWeaponItemData.h"

void UAnimNotify_DropMagazine::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (MeshComp && MeshComp->GetOwner())
    {
        AAACharacterPlayer* Character = Cast<AAACharacterPlayer>(MeshComp->GetOwner());
        if (Character)
        {
            Character->DropMagazine();
            Character->PlayRemoveMagSound();
        }
    }
}