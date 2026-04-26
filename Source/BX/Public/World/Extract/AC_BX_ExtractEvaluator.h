// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "AC_BX_ExtractEvaluator.generated.h"

class UDataTable;
struct FBXExtractRuleRow;

// 条件評価結果 (SPEC §18-3-2)
USTRUCT(BlueprintType)
struct BX_API FBXExtractEvalResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    bool bCanUseNow = false;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    bool bMeetsKeyRequirement = true;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    bool bMeetsMerchantRequirement = true;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    bool bMeetsQuestRequirement = true;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    bool bMeetsStoryRequirement = true;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    bool bMeetsPowerRequirement = true;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    bool bMeetsAlertRequirement = true;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    bool bMeetsWeightRequirement = true;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    bool bMeetsTimeRequirement = true;

    // 未達成の条件タグ群 (UI 表示用)
    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    FGameplayTagContainer FailureReasonTags;
};

// 抽出条件評価コンポーネント (SPEC §18-2 / §18-3)
// ABXExtractPointBase に自動追加されるが、独立使用も可能
UCLASS(ClassGroup="BX", BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class BX_API UAC_BX_ExtractEvaluator : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Extract")
    TSoftObjectPtr<UDataTable> ExtractRuleDataTable;

    // 抽出条件を評価して結果を返す。
    // CarriedItemIds: プレイヤー所持アイテム ID 一覧 (鍵・消耗品チェック用)
    // ActivePowerTags: 現在通電している制御タグ
    // CurrentTimeTag: 現在の時刻帯タグ
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Extract")
    FBXExtractEvalResult EvaluateConditions(
        FName ExtractId,
        const TArray<FName>& CarriedItemIds,
        int32 AlertLevel,
        float PlayerWeightKg,
        const TArray<FName>& ActivePowerTags,
        FName CurrentTimeTag) const;

    // DataTable から指定 ExtractId の行を取得する
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Extract")
    bool FindExtractRow(FName ExtractId, FBXExtractEvalResult& OutDefaultResult) const;

private:
    const FBXExtractRuleRow* GetRow(FName ExtractId) const;
};
