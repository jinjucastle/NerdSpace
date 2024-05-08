// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AACharacterStatWidget.h"
#include "Components/TextBlock.h"

void UAACharacterStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for (TFieldIterator<FNumericProperty> PropIt(FAACharacterStat::StaticStruct()); PropIt; ++PropIt)
	{
		const FName PropKey(PropIt->GetName());
		const FName TextCharacterControlName = *FString::Printf(TEXT("Txt%sCharacter"), *PropIt->GetName());

		UTextBlock* CharacterTextBlock = Cast<UTextBlock>(GetWidgetFromName(TextCharacterControlName));
		if (CharacterTextBlock)
		{
			CharacterLookup.Add(PropKey, CharacterTextBlock);
		}
	}

	/*for (TFieldIterator<FNumericProperty> PropIt(UAAWeaponItemData::StaticStruct()); PropIt; ++PropIt)
	{
		const FName PropKey(PropIt->GetName());
		const FName TextWeaponControlName = *FString::Printf(TEXT("Txt%sWeapon"), *PropIt->GetName());

		UTextBlock* WeaponTextBlock = Cast<UTextBlock>(GetWidgetFromName(TextWeaponControlName));
		if (WeaponTextBlock)
		{
			WeaponLookup.Add(PropKey, WeaponTextBlock);
		}
	}*/

}

void UAACharacterStatWidget::UpdateStat(const FAACharacterStat& CharacterStat, const UAAWeaponItemData& WeaponStat)
{
	/*for (TFieldIterator<FNumericProperty> PropIt(FAACharacterStat::StaticStruct()); PropIt; ++PropIt)
	{
		const FName PropKey(PropIt->GetName());

		float BaseData = 0.0f;
		PropIt->GetValue_InContainer((const void*)&CharacterStat, &BaseData);
	

		UTextBlock** CharacterTextBlockPtr = CharacterLookup.Find(PropKey);
		if (CharacterTextBlockPtr)
		{
			(*CharacterTextBlockPtr)->SetText(FText::FromString(FString::SanitizeFloat(BaseData)));
		}
	}*/
}
