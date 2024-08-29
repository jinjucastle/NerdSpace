// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_FootPlaySound.h"
#include "Character/AACharacterPlayer.h"

void UAnimNotify_FootPlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (MeshComp && MeshComp->GetOwner())
    {
        AAACharacterPlayer* Character = Cast<AAACharacterPlayer>(MeshComp->GetOwner());
        if (Character)
        {
            Character->PlayFootSound();
        }
    }
}
