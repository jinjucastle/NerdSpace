// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LoadingMapGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NERDSPACE_API ALoadingMapGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	//ver0.13.1b
	//feat: shaderComplieCheck
	virtual void PostInitializeComponents() override;
private:

	FTimerHandle CheckShaderTimer;
	
	void CheckShader();
};
