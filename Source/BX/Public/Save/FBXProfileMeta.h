// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/BXEnums.h"

#include "FBXProfileMeta.generated.h"

// プロファイル識別・累計統計 (SPEC_GAPS G-01 / CLAUDE.md §6-2)
USTRUCT(BlueprintType)
struct BX_API FBXProfileMeta
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Profile")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Profile")
    FGuid ProfileId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Profile")
    FString ProfileName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Profile")
    EBXGameMode GameMode = EBXGameMode::HardcoreReal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Profile")
    FDateTime CreatedAt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Profile")
    FDateTime LastPlayedAt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Profile")
    int32 TotalRaids = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Profile")
    int32 TotalDeaths = 0;
};
