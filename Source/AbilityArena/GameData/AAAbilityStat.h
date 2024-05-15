#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AAAbilityStat.generated.h"

USTRUCT(BlueprintType)
struct FAAAbilityStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FAAAbilityStat() : CurrentWeapon(0), NewWeapon(0), MaxHp(0.0f), RPM(0.0f), Damage(0.0f), MovementSpeed(0.0f), AmmoSpeed(0.0f), AmmoSize(0.0f), AmmoScale(0.0f), Acceleration(0.0f),
		ReloadSpeed(0.0f), SplashRound(0.0f), BloodDrain(0) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponType)
	int32 CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponType)
	int32 NewWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float RPM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AmmoSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AmmoSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AmmoScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float ReloadSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float SplashRound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	int32 BloodDrain;


	FAAAbilityStat operator+(const FAAAbilityStat& Other) const
	{
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);

		FAAAbilityStat Result;
		float* ResultPtr = reinterpret_cast<float*>(&Result);
		int32 StatNum = sizeof(FAAAbilityStat) / sizeof(float);
		for (int32 i = 0; i < StatNum; i++)
		{
			ResultPtr[i] = ThisPtr[i] + OtherPtr[i];
		}

		return Result;
	}
};