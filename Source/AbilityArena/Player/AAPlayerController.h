// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AAPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API AAAPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	AAAPlayerController();

protected:
	virtual void BeginPlay() override;
};
