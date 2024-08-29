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
class NERDSPACE_API UAACardSelectUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UAACardSelectUI(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

private:
	void LoadAllWidgetBlueprints();
	void CategorizeWidgetBlueprints(const FString& AssetPath, const FString& AssetName);

	TArray<TSubclassOf<UUserWidget>> DefaultAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> DefaultRandAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> PistolAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> PistolRandAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> AsultRifleAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> AsultRifleRandAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> ShotgunAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> ShotgunRandAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> PanzerfaustAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> PanzerfaustRandAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> SniperRifleAbilityWidgetClasses;
	TArray<TSubclassOf<UUserWidget>> SniperRifleRandAbilityWidgetClasses;

	TObjectPtr<UHorizontalBox> HorizontalBox;
	TObjectPtr<class AAACharacterPlayer> Owner;

	TArray<UUserWidget*> CardList;
	TSubclassOf<UUserWidget> JackpotCard;

public:
	UUserWidget* GetRandomWidget();
};
