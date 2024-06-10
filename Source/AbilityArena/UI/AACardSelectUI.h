// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "AACardSelectUI.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYARENA_API UAACardSelectUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UAACardSelectUI(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

private:
	TArray<TSubclassOf<UUserWidget>> DefaultAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> PistolAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> AsultRifleAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> ShotgunAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> PanzerfaustAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> SniperRifleAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> FunnyAbilityWidgetClasses;
	TObjectPtr<UHorizontalBox> HorizontalBox;
	TObjectPtr<class AAACharacterPlayer> Owner;
	TArray<UUserWidget*> CardList;

public:
	UUserWidget* GetRandomWidget();
};
