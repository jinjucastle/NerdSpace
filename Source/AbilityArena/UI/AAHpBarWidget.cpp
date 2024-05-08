// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AAHpBarWidget.h"
#include "Components/ProgressBar.h"


UAAHpBarWidget::UAAHpBarWidget(const FObjectInitializer& ObjectIntializer):Super(ObjectIntializer)
{
	MaxHp = -1.0f;
}

void UAAHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(HpProgressBar);

}

void UAAHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	ensure(MaxHp > 0.0f);
	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
}
