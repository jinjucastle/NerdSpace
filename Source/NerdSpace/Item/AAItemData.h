// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AAItemData.generated.h"

/**
 * 
 */
UCLASS()
class NERDSPACE_API UAAItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("AAItemData", GetFName());
	}

public:
};
