// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/BXEnums.h"

#include "FBXLocationRow.generated.h"

// DT_BX_Locations 行構造体 - レイドロケーション定義 (Sprint 25)
USTRUCT(BlueprintType)
struct BX_API FBXLocationRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Location")
    FName LocationId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Location")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Location")
    EBXLocationDifficulty Difficulty = EBXLocationDifficulty::Normal;

    // レイド制限時間 (秒)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Location")
    float MaxRaidTimeSec = 2400.0f;

    // 最低抽出ポイント数 (レベル設計上の目安)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Location")
    int32 MinExtractPoints = 2;

    // 入場コスト (Credits, 0 = 無料)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Location")
    int32 CreditsEntry = 0;

    // 有効/無効フラグ (無効時はロビーに表示しない)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Location")
    bool bIsAvailable = true;

    // 解放に必要な商人 Trust (0 = 最初から利用可)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Location")
    int32 RequiredTrustLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Location")
    FText Description;
};
