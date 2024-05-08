// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/AAUserWidget.h"
#include "AAHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API UAAHpBarWidget : public UAAUserWidget
{
	GENERATED_BODY()
	
public:
	//ver 0.5.1 C
	UAAHpBarWidget(const FObjectInitializer& ObjectIntializer);

protected:
	virtual void NativeConstruct() override;

public:
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }
	void UpdateHpBar(float NewCurrentHp);

protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	float MaxHp;
};
