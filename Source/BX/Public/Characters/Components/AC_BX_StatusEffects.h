// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/BXEnums.h"

#include "AC_BX_StatusEffects.generated.h"

class UDataTable;

// 実行中の状態異常インスタンス (内部管理用)
USTRUCT(BlueprintType)
struct BX_API FBXActiveStatus
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="BX|Status")
    FName StatusId;

    UPROPERTY(BlueprintReadOnly, Category="BX|Status")
    EBXStatusType StatusType = EBXStatusType::Pain;

    // 残り持続時間 (bUsesDuration=false の場合は -1)
    UPROPERTY(BlueprintReadOnly, Category="BX|Status")
    float RemainingDurationSec = -1.0f;

    UPROPERTY(BlueprintReadOnly, Category="BX|Status")
    int32 StackCount = 1;

    // 次の Tick までの残り時間
    UPROPERTY(BlueprintReadOnly, Category="BX|Status")
    float NextTickCountdown = 0.0f;
};

// 状態異常コンポーネント (SPEC §15-2)
// - DataTable から DT_BX_StatusEffects を参照して効果を適用
// - Neuro Critical は 15 秒タイマー + レイド 1 回限り
// - 大出血は小出血を上書き
UCLASS(ClassGroup="BX", BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class BX_API UAC_BX_StatusEffects : public UActorComponent
{
    GENERATED_BODY()

public:
    UAC_BX_StatusEffects();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Status")
    TSoftObjectPtr<UDataTable> StatusEffectDataTable;

    // 状態異常を付与する。大出血付与時は小出血を除去する。
    UFUNCTION(BlueprintCallable, Category="BX|Status")
    void ApplyStatus(FName StatusId);

    // RequiredMedItemId を消費して対象状態異常を治療する
    UFUNCTION(BlueprintCallable, Category="BX|Status")
    bool TreatStatus(FName StatusId, FName MedItemId);

    // 指定状態異常を即時除去する (BP/デバッグ用)
    UFUNCTION(BlueprintCallable, Category="BX|Status")
    void RemoveStatus(FName StatusId);

    // 指定タイプの状態異常が存在するか
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Status")
    bool HasStatusOfType(EBXStatusType Type) const;

    // 現在の移動速度倍率を全アクティブ効果から合成する
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Status")
    float GetCombinedMoveSpeedRatio() const;

    // Neuro Critical が発動中か
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Status")
    bool IsNeuroCriticalActive() const;

    // レイド終了時にリセット (bNeuroCriticalUsed を含む)
    UFUNCTION(BlueprintCallable, Category="BX|Status")
    void ResetForNewRaid();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TArray<FBXActiveStatus> ActiveStatuses;

    // Neuro Critical がこのレイドで既に発動済みか
    bool bNeuroCriticalUsed = false;

    // Neuro Critical 残り時間 (アクティブでなければ 0)
    float NeuroCriticalCountdown = 0.0f;

    // DataTable から行を取得するヘルパー
    const struct FBXStatusEffectTableRow* FindRow(FName StatusId) const;

    // Tick ダメージなど効果を適用する
    void ApplyTickEffect(const struct FBXStatusEffectTableRow& Row, FBXActiveStatus& Status);

    // 大出血付与時に小出血を除去する
    void EvictSmallBleedIfLargeBleed(FName IncomingStatusId);

    // 乗算で各倍率を合成するヘルパー
    float CombineRatios(TFunctionRef<float(const struct FBXStatusEffectTableRow&)> Getter) const;
};
