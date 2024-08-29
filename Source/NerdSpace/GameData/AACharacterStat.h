#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AACharacterStat.generated.h"

USTRUCT(BlueprintType)
struct FAACharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FAACharacterStat() : MaxHp(0.0f), RPM(0.0f), MovementSpeed(0.0f) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float RPM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MovementSpeed;


	FAACharacterStat operator+(const FAACharacterStat& Other) const
	{
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);

		FAACharacterStat Result;
		float* ResultPtr = reinterpret_cast<float*>(&Result);
		int32 StatNum = sizeof(FAACharacterStat) / sizeof(float);
		for (int32 i = 0; i < StatNum; i++)
		{
			ResultPtr[i] = ThisPtr[i] + OtherPtr[i];
		}

		return Result;
	}
};