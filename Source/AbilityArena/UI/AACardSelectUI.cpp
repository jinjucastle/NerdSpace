// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AACardSelectUI.h"
#include "Character/AACharacterPlayer.h"
#include "Item/AAWeaponItemData.h"
#include "Algo/RandomShuffle.h"

UAACardSelectUI::UAACardSelectUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_DEF_AmmoSpeedUp(TEXT("/Game/UI/Card/Ability/WBP_DEF_AmmoSpeedUp.WBP_DEF_AmmoSpeedUp_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_DEF_DamageUp(TEXT("/Game/UI/Card/Ability/WBP_DEF_DamageUp.WBP_DEF_DamageUp_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_DEF_DelayUp(TEXT("/Game/UI/Card/Ability/WBP_DEF_DelayUp.WBP_DEF_DelayUp_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_DEF_MaxHpUpMoveSpeedDown(TEXT("/Game/UI/Card/Ability/WBP_DEF_MaxHpUpMoveSpeedDown.WBP_DEF_MaxHpUpMoveSpeedDown_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_DEF_MoveSpeedUp(TEXT("/Game/UI/Card/Ability/WBP_DEF_MoveSpeedUp.WBP_DEF_MoveSpeedUp_C"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_PST_AmmoScaleUpDamageUpAmmoSpeedDown(TEXT("/Game/UI/Card/Ability/WBP_PST_AmmoScaleUpDamageUpAmmoSpeedDown.WBP_PST_AmmoScaleUpDamageUpAmmoSpeedDown_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_PST_AmmoSizeUpDamageDown(TEXT("/Game/UI/Card/Ability/WBP_PST_AmmoSizeUpDamageDown.WBP_PST_AmmoSizeUpDamageDown_C"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_AR_AmmoSpeedUp(TEXT("/Game/UI/Card/Ability/WBP_AR_AmmoSpeedUp.WBP_AR_AmmoSpeedUp_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_AR_DamageUpAmmoSizeDown(TEXT("/Game/UI/Card/Ability/WBP_AR_DamageUpAmmoSizeDown.WBP_AR_DamageUpAmmoSizeDown_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_AR_DamageUpDelayDown(TEXT("/Game/UI/Card/Ability/WBP_AR_DamageUpDelayDown.WBP_AR_DamageUpDelayDown_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_AR_MoveSpeedUp(TEXT("/Game/UI/Card/Ability/WBP_AR_MoveSpeedUp.WBP_AR_MoveSpeedUp_C"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_SG_BloodDrainHpDown(TEXT("/Game/UI/Card/Ability/WBP_SG_BloodDrainHpDown.WBP_SG_BloodDrainHpDown_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_SG_DamageUpMoveSpeedDown(TEXT("/Game/UI/Card/Ability/WBP_SG_DamageUpMoveSpeedDown.WBP_SG_DamageUpMoveSpeedDown_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_SG_DamageUpReloadSpeedDown(TEXT("/Game/UI/Card/Ability/WBP_SG_DamageUpReloadSpeedDown.WBP_SG_DamageUpReloadSpeedDown_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_SG_MoveSpeedUp(TEXT("/Game/UI/Card/Ability/WBP_SG_MoveSpeedUp.WBP_SG_MoveSpeedUp_C"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_PZF_AccelerationUp(TEXT("/Game/UI/Card/Ability/WBP_PZF_AccelerationUp.WBP_PZF_AccelerationUp_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_PZF_DamageUpSplashDown(TEXT("/Game/UI/Card/Ability/WBP_PZF_DamageUpSplashDown.WBP_PZF_DamageUpSplashDown_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_PZF_ReloadSpeedUp(TEXT("/Game/UI/Card/Ability/WBP_PZF_ReloadSpeedUp.WBP_PZF_ReloadSpeedUp_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_PZF_SplashUpDamageDown(TEXT("/Game/UI/Card/Ability/WBP_PZF_SplashUpDamageDown.WBP_PZF_SplashUpDamageDown_C"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_SR_AmmoScaleUp(TEXT("/Game/UI/Card/Ability/WBP_SR_AmmoScaleUp.WBP_SR_AmmoScaleUp_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_SR_AmmoSizeUp(TEXT("/Game/UI/Card/Ability/WBP_SR_AmmoSizeUp.WBP_SR_AmmoSizeUp_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_SR_MoveSpeedUp(TEXT("/Game/UI/Card/Ability/WBP_SR_MoveSpeedUp.WBP_SR_MoveSpeedUp_C"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_ChangeWeapon_ToAR(TEXT("/Game/UI/Card/ChangeWeapon/WBP_ChangeWeapon_ToAR.WBP_ChangeWeapon_ToAR_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_ChangeWeapon_ToFN(TEXT("/Game/UI/Card/ChangeWeapon/WBP_ChangeWeapon_ToFN.WBP_ChangeWeapon_ToFN_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_ChangeWeapon_ToPZF(TEXT("/Game/UI/Card/ChangeWeapon/WBP_ChangeWeapon_ToPZF.WBP_ChangeWeapon_ToPZF_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_ChangeWeapon_ToSG(TEXT("/Game/UI/Card/ChangeWeapon/WBP_ChangeWeapon_ToSG.WBP_ChangeWeapon_ToSG_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_ChangeWeapon_ToSR(TEXT("/Game/UI/Card/ChangeWeapon/WBP_ChangeWeapon_ToSR.WBP_ChangeWeapon_ToSR_C"));
	
	if (WBP_DEF_AmmoSpeedUp.Succeeded()) DefaultAbilityWidgetClasses.Add(WBP_DEF_AmmoSpeedUp.Class);
	if (WBP_DEF_DamageUp.Succeeded()) DefaultAbilityWidgetClasses.Add(WBP_DEF_DamageUp.Class);
	if (WBP_DEF_DelayUp.Succeeded()) DefaultAbilityWidgetClasses.Add(WBP_DEF_DelayUp.Class);
	if (WBP_DEF_MaxHpUpMoveSpeedDown.Succeeded()) DefaultAbilityWidgetClasses.Add(WBP_DEF_MaxHpUpMoveSpeedDown.Class);
	if (WBP_DEF_MoveSpeedUp.Succeeded()) DefaultAbilityWidgetClasses.Add(WBP_DEF_MoveSpeedUp.Class);

	if (WBP_PST_AmmoScaleUpDamageUpAmmoSpeedDown.Succeeded()) PistolAbilityWidgetClasses.Add(WBP_PST_AmmoScaleUpDamageUpAmmoSpeedDown.Class);
	if (WBP_PST_AmmoSizeUpDamageDown.Succeeded()) PistolAbilityWidgetClasses.Add(WBP_PST_AmmoSizeUpDamageDown.Class);
	if (WBP_ChangeWeapon_ToAR.Succeeded()) PistolAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToAR.Class);
	if (WBP_ChangeWeapon_ToFN.Succeeded()) PistolAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToFN.Class);
	if (WBP_ChangeWeapon_ToPZF.Succeeded()) PistolAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToPZF.Class);
	if (WBP_ChangeWeapon_ToSG.Succeeded()) PistolAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToSG.Class);
	if (WBP_ChangeWeapon_ToSR.Succeeded()) PistolAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToSR.Class);

	if (WBP_AR_AmmoSpeedUp.Succeeded()) AsultRifleAbilityWidgetClasses.Add(WBP_AR_AmmoSpeedUp.Class);
	if (WBP_AR_DamageUpAmmoSizeDown.Succeeded()) AsultRifleAbilityWidgetClasses.Add(WBP_AR_DamageUpAmmoSizeDown.Class);
	if (WBP_AR_DamageUpDelayDown.Succeeded()) AsultRifleAbilityWidgetClasses.Add(WBP_AR_DamageUpDelayDown.Class);
	if (WBP_AR_MoveSpeedUp.Succeeded()) AsultRifleAbilityWidgetClasses.Add(WBP_AR_MoveSpeedUp.Class);
	if (WBP_ChangeWeapon_ToFN.Succeeded()) AsultRifleAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToFN.Class);
	if (WBP_ChangeWeapon_ToPZF.Succeeded()) AsultRifleAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToPZF.Class);
	if (WBP_ChangeWeapon_ToSG.Succeeded()) AsultRifleAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToSG.Class);
	if (WBP_ChangeWeapon_ToSR.Succeeded()) AsultRifleAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToSR.Class);

	if (WBP_SG_BloodDrainHpDown.Succeeded()) ShotgunAbilityWidgetClasses.Add(WBP_SG_BloodDrainHpDown.Class);
	if (WBP_SG_DamageUpMoveSpeedDown.Succeeded()) ShotgunAbilityWidgetClasses.Add(WBP_SG_DamageUpMoveSpeedDown.Class);
	if (WBP_SG_DamageUpReloadSpeedDown.Succeeded()) ShotgunAbilityWidgetClasses.Add(WBP_SG_DamageUpReloadSpeedDown.Class);
	if (WBP_SG_MoveSpeedUp.Succeeded()) ShotgunAbilityWidgetClasses.Add(WBP_SG_MoveSpeedUp.Class);
	if (WBP_ChangeWeapon_ToAR.Succeeded()) ShotgunAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToAR.Class);
	if (WBP_ChangeWeapon_ToFN.Succeeded()) ShotgunAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToFN.Class);
	if (WBP_ChangeWeapon_ToPZF.Succeeded()) ShotgunAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToPZF.Class);
	if (WBP_ChangeWeapon_ToSR.Succeeded()) ShotgunAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToSR.Class);

	if (WBP_PZF_AccelerationUp.Succeeded()) PanzerfaustAbilityWidgetClasses.Add(WBP_PZF_AccelerationUp.Class);
	if (WBP_PZF_DamageUpSplashDown.Succeeded()) PanzerfaustAbilityWidgetClasses.Add(WBP_PZF_DamageUpSplashDown.Class);
	if (WBP_PZF_ReloadSpeedUp.Succeeded()) PanzerfaustAbilityWidgetClasses.Add(WBP_PZF_ReloadSpeedUp.Class);
	if (WBP_PZF_SplashUpDamageDown.Succeeded()) PanzerfaustAbilityWidgetClasses.Add(WBP_PZF_SplashUpDamageDown.Class);
	if (WBP_ChangeWeapon_ToAR.Succeeded()) PanzerfaustAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToAR.Class);
	if (WBP_ChangeWeapon_ToFN.Succeeded()) PanzerfaustAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToFN.Class);
	if (WBP_ChangeWeapon_ToSG.Succeeded()) PanzerfaustAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToSG.Class);
	if (WBP_ChangeWeapon_ToSR.Succeeded()) PanzerfaustAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToSR.Class);

	if (WBP_SR_AmmoScaleUp.Succeeded()) SniperRifleAbilityWidgetClasses.Add(WBP_SR_AmmoScaleUp.Class);
	if (WBP_SR_AmmoSizeUp.Succeeded()) SniperRifleAbilityWidgetClasses.Add(WBP_SR_AmmoSizeUp.Class);
	if (WBP_SR_MoveSpeedUp.Succeeded()) SniperRifleAbilityWidgetClasses.Add(WBP_SR_MoveSpeedUp.Class);
	if (WBP_ChangeWeapon_ToAR.Succeeded()) SniperRifleAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToAR.Class);
	if (WBP_ChangeWeapon_ToFN.Succeeded()) SniperRifleAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToFN.Class);
	if (WBP_ChangeWeapon_ToSG.Succeeded()) SniperRifleAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToSG.Class);
	if (WBP_ChangeWeapon_ToPZF.Succeeded()) SniperRifleAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToPZF.Class);

	if (WBP_ChangeWeapon_ToAR.Succeeded()) FunnyAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToAR.Class);
	if (WBP_ChangeWeapon_ToSG.Succeeded()) FunnyAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToSG.Class);
	if (WBP_ChangeWeapon_ToPZF.Succeeded()) FunnyAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToPZF.Class);
	if (WBP_ChangeWeapon_ToSR.Succeeded()) FunnyAbilityWidgetClasses.Add(WBP_ChangeWeapon_ToSR.Class);

}

void UAACardSelectUI::NativeConstruct()
{
	Super::NativeConstruct();

	Owner = Cast<AAACharacterPlayer>(GetOwningPlayerPawn());
	
	bool isSucceedLoad = (DefaultAbilityWidgetClasses.Num() + PistolAbilityWidgetClasses.Num() + AsultRifleAbilityWidgetClasses.Num() + ShotgunAbilityWidgetClasses.Num() +
		PanzerfaustAbilityWidgetClasses.Num() + SniperRifleAbilityWidgetClasses.Num() + FunnyAbilityWidgetClasses.Num() >= 4);

	if (!Owner)
	{
		UE_LOG(LogTemp, Log, TEXT("Can't Find Owner"));
		return;
	}

	HorizontalBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HorizontalBox")));

	if (HorizontalBox && isSucceedLoad)
	{
		TArray<TSubclassOf<UUserWidget>> CurrentTypeAbility;

		CurrentTypeAbility.Append(DefaultAbilityWidgetClasses);

		switch (Owner->GetWeaponData()->Type)
		{
		case EWeaponType::Pistol:
			CurrentTypeAbility.Append(PistolAbilityWidgetClasses);
			break;
		case EWeaponType::Rifle:
			CurrentTypeAbility.Append(AsultRifleAbilityWidgetClasses);
			break;
		case EWeaponType::Shotgun:
			CurrentTypeAbility.Append(ShotgunAbilityWidgetClasses);
			break;
		case EWeaponType::Panzerfaust:
			CurrentTypeAbility.Append(PanzerfaustAbilityWidgetClasses);
			break;
		case EWeaponType::SniperRifle:
			CurrentTypeAbility.Append(SniperRifleAbilityWidgetClasses);
			break;
		case EWeaponType::Funny:
			CurrentTypeAbility.Append(FunnyAbilityWidgetClasses);
			break;
		default:
			break;
		}

		Algo::RandomShuffle(CurrentTypeAbility);

		for (int32 i = 0; i < 4; i++)
		{
			UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), CurrentTypeAbility[i]);
			if (Widget)
			{
				UHorizontalBoxSlot* HorizontalSlot = HorizontalBox->AddChildToHorizontalBox(Widget);
				HorizontalSlot->SetPadding(FMargin(5.0f));
				HorizontalSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
			}
		}
	}
}
