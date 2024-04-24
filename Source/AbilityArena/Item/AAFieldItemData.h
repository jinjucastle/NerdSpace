// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/AAItemData.h"
#include "AAFieldItemData.generated.h"

UENUM(BlueprintType)
enum class EFieldType : uint8
{
	Recovery = 0,
	Shield
};

/**
 * 
 */
UCLASS()
class ABILITYARENA_API UAAFieldItemData : public UAAItemData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EFieldType Type;
	
};
