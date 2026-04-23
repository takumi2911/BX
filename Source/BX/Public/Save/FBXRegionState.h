// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "FBXRegionState.generated.h"

// 地域 1 区画分の状態フラグ (CLAUDE.md §6-2)
// イベント状態の詳細は §16-3 追記後に拡張 → docs/QUESTIONS.md Q-04
USTRUCT(BlueprintType)
struct BX_API FBXRegionState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Region")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Region")
    FName RegionId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Region")
    bool bIsDiscovered = false;

    // 地域内イベント・フラグ（抽出解放・エリア封鎖など）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Region")
    TArray<FName> EventStateFlags;
};
