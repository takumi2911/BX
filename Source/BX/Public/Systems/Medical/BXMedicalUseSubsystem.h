// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "BXMedicalUseSubsystem.generated.h"

class UAC_BX_StatusEffects;
class UDataTable;

// 医療アイテム使用サブシステム (SPEC §15-3)
// - UseTimeSec に従って使用タイマーを管理
// - CooldownSec を TMap<FName, FTimerHandle> で管理
// - 使用完了時に状態異常解除・バフ付与を AC_BX_StatusEffects 経由で適用
// - 中断条件 (被弾/ダッシュ/武器使用) は BP から NotifyXxx メソッドを呼ぶ
UCLASS()
class BX_API UBXMedicalUseSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Medical")
    TSoftObjectPtr<UDataTable> MedicalItemDataTable;

    // 医療アイテムの使用を開始する。成功時 true を返す。
    UFUNCTION(BlueprintCallable, Category="BX|Medical")
    bool UseItem(FName MedicalItemId, UAC_BX_StatusEffects* StatusComp);

    // 現在の使用をキャンセルする
    UFUNCTION(BlueprintCallable, Category="BX|Medical")
    void CancelCurrentUse();

    // 使用可否チェック
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Medical")
    bool CanUseItem(FName MedicalItemId) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Medical")
    bool IsCurrentlyUsing() const;

    // 使用進捗 (0.0〜1.0)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Medical")
    float GetUseProgressRatio() const;

    // 現在使用中のアイテム ID (使用中でなければ NAME_None)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Medical")
    FName GetCurrentUsingItemId() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Medical")
    bool IsOnCooldown(FName MedicalItemId) const;

    // クールダウン残り比率 (0.0 = クールダウン完了 or 未設定, 1.0 = 開始直後)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Medical")
    float GetCooldownRemainingRatio(FName MedicalItemId) const;

    // --- 中断通知 (BP から被弾 / ダッシュ / 武器使用時に呼ぶ) ---
    UFUNCTION(BlueprintCallable, Category="BX|Medical")
    void NotifyDamageReceived();

    UFUNCTION(BlueprintCallable, Category="BX|Medical")
    void NotifySprintStarted();

    UFUNCTION(BlueprintCallable, Category="BX|Medical")
    void NotifyWeaponFired();

    // レイド終了時にリセット
    UFUNCTION(BlueprintCallable, Category="BX|Medical")
    void ResetForNewRaid();

private:
    // 現在使用中のアイテム ID (使用中でなければ NAME_None)
    FName CurrentItemId = NAME_None;

    // 使用開始時刻 (GetWorld()->GetTimeSeconds())
    float UseStartTimeSec = 0.0f;

    // 使用中アイテムの UseTimeSec
    float CurrentUseTimeSec = 0.0f;

    // 使用中の StatusEffects コンポーネント参照
    UPROPERTY()
    TObjectPtr<UAC_BX_StatusEffects> CurrentStatusComp;

    // クールダウンタイマーハンドル (key: MedicalItemId)
    TMap<FName, FTimerHandle> CooldownTimers;

    // アイテム使用完了タイマー
    FTimerHandle UseCompleteTimer;

    // 使用完了コールバック
    UFUNCTION()
    void OnUseComplete();

    // DataTable 行を取得
    const struct FBXMedicalItemRow* FindRow(FName MedicalItemId) const;

    // クールダウン開始
    void StartCooldown(FName MedicalItemId, float CooldownSec);
};
