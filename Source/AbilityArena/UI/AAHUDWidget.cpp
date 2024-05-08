// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AAHUDWidget.h"
#include "Interface/AACharacterHUDInterface.h"
#include "AAHpBarWidget.h"
#include "AACharacterStatWidget.h"

UAAHUDWidget::UAAHUDWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
}

void UAAHUDWidget::UpdateStat(const FAACharacterStat& CharacterStat, const UAAWeaponItemData& WeaponStat)
{
	HpBar->SetMaxHp(CharacterStat.MaxHp);

	CharacterAndWeaponStat->UpdateStat(CharacterStat, WeaponStat);
}

void UAAHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	HpBar->UpdateHpBar(NewCurrentHp);
}

void UAAHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpBar = Cast<UAAHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensure(HpBar);

	CharacterAndWeaponStat = Cast<UAACharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));
	ensure(CharacterAndWeaponStat);

	IAACharacterHUDInterface* HUDPawn = Cast<IAACharacterHUDInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}
