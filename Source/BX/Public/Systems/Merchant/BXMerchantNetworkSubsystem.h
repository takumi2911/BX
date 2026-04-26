// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Save/FBXMerchantNetworkState.h"

#include "BXMerchantNetworkSubsystem.generated.h"

class UDataTable;
struct FBXMerchantTableRow;
struct FBXMerchantTrustRewardRow;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTrustLevelChanged, FName, MerchantId, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRewardUnlocked, FName, MerchantId, FName, RewardId);

// 商人ネットワーク管理サブシステム (SPEC §21-1)
// - Trust 累積 / 閾値判定 / レベルアップ
// - 解放条件評価 (Trust + 依頼 + ストーリーフラグ)
// - SaveGame の FBXMerchantNetworkState と同期
UCLASS()
class BX_API UBXMerchantNetworkSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Merchant")
    TSoftObjectPtr<UDataTable> MerchantDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Merchant")
    TSoftObjectPtr<UDataTable> TrustRewardDataTable;

    // --- Trust ---

    UFUNCTION(BlueprintCallable, Category="BX|Merchant")
    void AddTrustPoints(FName MerchantId, int32 Points);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Merchant")
    int32 GetTrustLevel(FName MerchantId) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Merchant")
    int32 GetTrustPoints(FName MerchantId) const;

    // 現レベル内での進捗比率 (0.0 = 基点, 1.0 = 次レベル閾値)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Merchant")
    float GetTrustProgressRatio(FName MerchantId) const;

    // --- Rewards ---

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Merchant")
    bool IsRewardUnlocked(FName RewardId) const;

    // MerchantId の解放済み報酬行名リスト (PriorityOrder 昇順)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Merchant")
    TArray<FName> GetUnlockedRewardIds(FName MerchantId) const;

    // bVisibleBeforeUnlock=true かつ未解放の報酬行名リスト (UI 予告用)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Merchant")
    TArray<FName> GetVisiblePendingRewardIds(FName MerchantId) const;

    // --- Contact ---

    // StoryFlagRequiredForContact を満たしているか
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Merchant")
    bool IsContactUnlocked(FName MerchantId) const;

    // --- State signals ---

    // 依頼完了を通知 → 関連報酬条件を再評価
    UFUNCTION(BlueprintCallable, Category="BX|Merchant")
    void SetQuestCompleted(FName QuestId, FName MerchantId);

    // ストーリーフラグを有効化 → 全商人の報酬条件を再評価
    UFUNCTION(BlueprintCallable, Category="BX|Merchant")
    void SetStoryFlagActive(FName StoryFlag);

    // --- SaveGame 連携 ---

    UFUNCTION(BlueprintCallable, Category="BX|Merchant")
    void SaveToNetworkState(UPARAM(ref) FBXMerchantNetworkState& OutState) const;

    UFUNCTION(BlueprintCallable, Category="BX|Merchant")
    void LoadFromNetworkState(const FBXMerchantNetworkState& InState);

    UFUNCTION(BlueprintCallable, Category="BX|Merchant")
    void ResetForNewProfile();

    // --- Delegates ---

    UPROPERTY(BlueprintAssignable, Category="BX|Merchant")
    FOnTrustLevelChanged OnTrustLevelChanged;

    UPROPERTY(BlueprintAssignable, Category="BX|Merchant")
    FOnRewardUnlocked OnRewardUnlocked;

private:
    FBXMerchantNetworkState NetworkState;
    TSet<FName> ActiveStoryFlags;
    TSet<FName> AppliedRewardIds;

    FBXMerchantState& GetOrCreateMerchantState(FName MerchantId);
    int32 ComputeTrustLevel(FName MerchantId, int32 TrustPoints) const;
    void CheckAndApplyNewRewards(FName MerchantId);
    void ApplyReward(FName MerchantId, const FBXMerchantTrustRewardRow& Row);
    bool AreRewardConditionsMet(FName MerchantId, const FBXMerchantTrustRewardRow& Row) const;
    const FBXMerchantTableRow* FindMerchantRow(FName MerchantId) const;
};
