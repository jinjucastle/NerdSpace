// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/AAFieldItemData.h"
#include "AARecoveryItem.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API UAARecoveryItem : public UAAFieldItemData
{
	GENERATED_BODY()
public:
	UAARecoveryItem();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	float RecoveryAmount;
};
