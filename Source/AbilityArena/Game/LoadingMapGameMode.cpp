// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LoadingMapGameMode.h"
#include "ShaderCompiler.h"
#include "ShaderCompilerCore.h"
#include "Kismet/GamePlayStatics.h"


//0.13.1b
//feat: Checking shaderCompile
void ALoadingMapGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetWorldTimerManager().SetTimer(CheckShaderTimer, this, &ALoadingMapGameMode::CheckShader, 1.0f, true);
}

void ALoadingMapGameMode::CheckShader()
{
	if (!GShaderCompilingManager->IsCompiling())
	{
		FString ChangeMap = TEXT("/Game/Maps/TestJoinMap");

		UGameplayStatics::OpenLevel(GetWorld(),*ChangeMap,true);
		GetWorld()->GetTimerManager().ClearTimer(CheckShaderTimer);
	}

}
