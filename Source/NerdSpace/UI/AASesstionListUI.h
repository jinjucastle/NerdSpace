// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "AASesstionListUI.generated.h"

/**
 * 
 */
UCLASS()
class NERDSPACE_API UAASesstionListUI : public UUserWidget
{
	GENERATED_BODY()
	

public:


	UFUNCTION(BlueprintCallable)
	void GetSessionInfo(FString SessionName);

};
