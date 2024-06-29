// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AACardSelectUI.h"
#include "Character/AACharacterPlayer.h"
#include "Item/AAWeaponItemData.h"
#include "Algo/RandomShuffle.h"
#include "Engine/AssetManager.h"

UAACardSelectUI::UAACardSelectUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UAACardSelectUI::NativeConstruct()
{
	Super::NativeConstruct();

	Owner = Cast<AAACharacterPlayer>(GetOwningPlayerPawn());

	LoadAllWidgetBlueprints();

	bool isSucceedLoad = (DefaultAbilityWidgetClasses.Num() + PistolAbilityWidgetClasses.Num() + AsultRifleAbilityWidgetClasses.Num() + ShotgunAbilityWidgetClasses.Num() +
		PanzerfaustAbilityWidgetClasses.Num() + SniperRifleAbilityWidgetClasses.Num() >= 4);

	if (!Owner)
	{
		UE_LOG(LogTemp, Log, TEXT("Can't Find Owner"));
		return;
	}

	HorizontalBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HorizontalBox")));

	if (HorizontalBox && isSucceedLoad)
	{
		TArray<TSubclassOf<UUserWidget>> CurrentTypeAbility;
		TArray<TSubclassOf<UUserWidget>> CurrentTypeRandAbility;

		CurrentTypeAbility.Append(DefaultAbilityWidgetClasses);
		CurrentTypeRandAbility.Append(DefaultRandAbilityWidgetClasses);

		switch (Owner->GetWeaponData()->Type)
		{
		case EWeaponType::Pistol:
			CurrentTypeAbility.Append(PistolAbilityWidgetClasses);
			CurrentTypeRandAbility.Append(PistolRandAbilityWidgetClasses);
			break;
		case EWeaponType::Rifle:
			CurrentTypeAbility.Append(AsultRifleAbilityWidgetClasses);
			CurrentTypeRandAbility.Append(AsultRifleRandAbilityWidgetClasses);
			break;
		case EWeaponType::Shotgun:
			CurrentTypeAbility.Append(ShotgunAbilityWidgetClasses);
			CurrentTypeRandAbility.Append(ShotgunRandAbilityWidgetClasses);
			break;
		case EWeaponType::Panzerfaust:
			CurrentTypeAbility.Append(PanzerfaustAbilityWidgetClasses);
			CurrentTypeRandAbility.Append(PanzerfaustRandAbilityWidgetClasses);
			break;
		case EWeaponType::SniperRifle:
			CurrentTypeAbility.Append(SniperRifleAbilityWidgetClasses);
			CurrentTypeRandAbility.Append(SniperRifleRandAbilityWidgetClasses);
			break;
		default:
			break;
		}

		Algo::RandomShuffle(CurrentTypeAbility);
		Algo::RandomShuffle(CurrentTypeRandAbility);

		UE_LOG(LogTemp, Log, TEXT("Found %d assets"), CurrentTypeAbility.Num() + CurrentTypeRandAbility.Num());

		for (int32 i = 0; i < 4; i++)
		{
			UUserWidget* Widget;

			float Chance = FMath::FRand();

			if (Chance <= 0.05f)
			{
				if (Chance <= 0.01f)
				{
					Widget = CreateWidget<UUserWidget>(GetWorld(), JackpotCard);
				}
				else
				{
					Widget = CreateWidget<UUserWidget>(GetWorld(), CurrentTypeRandAbility[i]);
				}
			}
			else
			{
				Widget = CreateWidget<UUserWidget>(GetWorld(), CurrentTypeAbility[i]);
			}
			
			if (Widget)
			{
				UHorizontalBoxSlot* HorizontalSlot = HorizontalBox->AddChildToHorizontalBox(Widget);
				HorizontalSlot->SetPadding(FMargin(5.0f));
				HorizontalSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
				CardList.Add(Widget);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Can't Find File"));
	}
}

void UAACardSelectUI::LoadAllWidgetBlueprints()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetDataArray;

	// 특정 경로의 에셋을 검색
	FString WidgetFolderPath = "/Game/UI/Card";
	AssetRegistry.ScanPathsSynchronous({ WidgetFolderPath }, true);
	AssetRegistry.GetAssetsByPath(FName(*WidgetFolderPath), AssetDataArray, true);

	// 디버깅 로그: 경로와 발견된 에셋 개수
	UE_LOG(LogTemp, Log, TEXT("Scanning Path: %s"), *WidgetFolderPath);
	UE_LOG(LogTemp, Log, TEXT("Found %d assets"), AssetDataArray.Num());

	for (const FAssetData& AssetData : AssetDataArray)
	{
		FString AssetName = AssetData.AssetName.ToString();
		FString AssetPath = AssetData.GetObjectPathString();
		CategorizeWidgetBlueprints(AssetPath, AssetName);
	}
}

void UAACardSelectUI::CategorizeWidgetBlueprints(const FString& AssetPath, const FString& AssetName)
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	FSoftObjectPath WidgetClassPath(AssetPath);

	UClass* WidgetClass = Cast<UClass>(Streamable.LoadSynchronous(WidgetClassPath));
	if (WidgetClass && WidgetClass->IsChildOf(UUserWidget::StaticClass()))
	{
		TSubclassOf<UUserWidget> UserWidgetClass = WidgetClass;
		if (AssetName.Contains("WBP_DEF_") || AssetName.Contains("WBP_ChangeWeapon"))
		{
			if (AssetName.Contains("JACKPOT"))
			{
				JackpotCard = UserWidgetClass;
				return;
			}

			if (AssetName.Contains("Rand"))
			{
				DefaultRandAbilityWidgetClasses.Add(UserWidgetClass);
				return;
			}

			switch (Owner->GetWeaponData()->Type)
			{
			case EWeaponType::Rifle:
				if (AssetName.Contains("WBP_ChangeWeapon_ToAR")) return;
				break;
			case EWeaponType::Shotgun:
				if (AssetName.Contains("WBP_ChangeWeapon_ToSG")) return;
				break;
			case EWeaponType::Panzerfaust:
				if (AssetName.Contains("WBP_ChangeWeapon_ToPZF")) return;
				break;
			case EWeaponType::SniperRifle:
				if (AssetName.Contains("WBP_ChangeWeapon_ToSR")) return;
				break;
			}

			DefaultAbilityWidgetClasses.Add(UserWidgetClass);
		}
		else if (AssetName.Contains("WBP_PST_"))
		{
			if (AssetName.Contains("Rand"))
			{
				PistolRandAbilityWidgetClasses.Add(UserWidgetClass);
				return;
			}

			PistolAbilityWidgetClasses.Add(UserWidgetClass);
		}
		else if (AssetName.Contains("WBP_AR_"))
		{
			if (AssetName.Contains("Rand"))
			{
				AsultRifleRandAbilityWidgetClasses.Add(UserWidgetClass);
				return;
			}

			AsultRifleAbilityWidgetClasses.Add(UserWidgetClass);
		}
		else if (AssetName.Contains("WBP_SG_"))
		{
			if (AssetName.Contains("Rand"))
			{
				ShotgunRandAbilityWidgetClasses.Add(UserWidgetClass);
				return;
			}

			ShotgunAbilityWidgetClasses.Add(UserWidgetClass);
		}
		else if (AssetName.Contains("WBP_PZF_"))
		{
			if (AssetName.Contains("Rand"))
			{
				PanzerfaustRandAbilityWidgetClasses.Add(UserWidgetClass);
				return;
			}

			PanzerfaustAbilityWidgetClasses.Add(UserWidgetClass);
		}
		else if (AssetName.Contains("WBP_SR_"))
		{
			if (AssetName.Contains("Rand"))
			{
				SniperRifleRandAbilityWidgetClasses.Add(UserWidgetClass);
				return;
			}

			SniperRifleAbilityWidgetClasses.Add(UserWidgetClass);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load widget: %s"), *AssetPath);
	}
}

UUserWidget* UAACardSelectUI::GetRandomWidget()
{
	if (CardList.Num() > 0)
	{
		int32 RandomIdx = FMath::RandRange(0, CardList.Num() - 1);
		return CardList[RandomIdx];
	}
	return nullptr;
}
