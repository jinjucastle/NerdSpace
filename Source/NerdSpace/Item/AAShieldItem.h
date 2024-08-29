// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/AAFieldItemData.h"
#include "AAShieldItem.generated.h"

/**
 * 
 */
UCLASS()
class NERDSPACE_API UAAShieldItem : public UAAFieldItemData
{
	GENERATED_BODY()
public:
	UAAShieldItem();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	int32 ShieldCount;
};
