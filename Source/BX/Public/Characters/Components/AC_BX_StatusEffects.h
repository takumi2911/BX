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
// Sprint 20: GetCombinedAimStabilityRatio / GetCombinedADSSpeedRatio /
//            GetCombinedStaminaRecoveryRatio / GetActiveStatusTypes を追加
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

    // 照準安定倍率を全アクティブ効果から合成する — Sprint 20
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Status")
    float GetCombinedAimStabilityRatio() const;

    // ADS 速度倍率を全アクティブ効果から合成する — Sprint 20
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Status")
    float GetCombinedADSSpeedRatio() const;

    // スタミナ回復倍率を全アクティブ効果から合成する — Sprint 20
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Status")
    float GetCombinedStaminaRecoveryRatio() const;

    // 現在アクティブな状態異常タイプ一覧 (HUD アイコン更新用) — Sprint 20
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Status")
    TArray<EBXStatusType> GetActiveStatusTypes() const;

    // 現在アクティブな状態異常 ID 一覧 — Sprint 20
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Status")
    TArray<FName> GetActiveStatusIds() const;

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

    bool bNeuroCriticalUsed = false;
    float NeuroCriticalCountdown = 0.0f;

    const struct FBXStatusEffectTableRow* FindRow(FName StatusId) const;
    void ApplyTickEffect(const struct FBXStatusEffectTableRow& Row, FBXActiveStatus& Status);
    void EvictSmallBleedIfLargeBleed(FName IncomingStatusId);
    float CombineRatios(TFunctionRef<float(const struct FBXStatusEffectTableRow&)> Getter) const;
};
