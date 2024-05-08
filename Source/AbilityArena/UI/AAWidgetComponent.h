// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AAWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API UAAWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
protected:
	virtual void InitWidget() override;

};
