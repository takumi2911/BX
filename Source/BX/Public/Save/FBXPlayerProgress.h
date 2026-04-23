// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "FBXPlayerProgress.generated.h"

// プレイヤー進行状態（通貨・依頼・発見地域）(CLAUDE.md §6-2)
// 依頼進行の詳細フィールドは §16-3 追記後に拡張 → docs/QUESTIONS.md Q-03
USTRUCT(BlueprintType)
struct BX_API FBXPlayerProgress
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Progress")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Progress")
    int32 Credits = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Progress")
    TArray<FName> DiscoveredRegions;

    // ストーリー進行フラグ（依頼完了・地点解放などで付与）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Progress")
    TArray<FName> StoryFlags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Progress")
    int32 TotalRaidCount = 0;
};
