// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AACharacterItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAACharacterItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ABILITYARENA_API IAACharacterItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void TakeItem(class UAAItemData* InItemData) = 0;
};
