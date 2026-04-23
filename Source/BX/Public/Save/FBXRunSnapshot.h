// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Save/FBXMerchantNetworkState.h"

#include "FBXRunSnapshot.generated.h"

// レイド中断時の一時セーブ（条件付き）(CLAUDE.md §6-2)
// 中断セーブの詳細条件（どの状況で発生するか）は §16-3 追記後に拡張 → docs/QUESTIONS.md Q-05
USTRUCT(BlueprintType)
struct BX_API FBXRunSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Run")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Run")
    bool bHasActiveRun = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Run")
    FName RegionId;

    // レイド経過時間（秒）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Run")
    float RaidElapsedTimeSec = 0.0f;

    // 出撃時に持ち込んでいるアイテム（ロスト対象）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Run")
    TArray<FBXSavedItemStack> ItemsCarried;
};
