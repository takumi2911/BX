// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Quests/FBXQuestTableRow.h"
#include "GameplayTagContainer.h"

#include "BXQuestSubsystem.generated.h"

class UDataTable;
class UBXMerchantNetworkSubsystem;
struct FBXMerchantNetworkState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestAccepted,  FName, QuestId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, FName, QuestId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestFailed,    FName, QuestId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestProgress, FName, QuestId, int32, NewCount);

// 依頼管理サブシステム (SPEC §21-2)
// - 受注条件チェック / 進捗更新 / 完了 / 失敗 / 報酬配布
// - UBXMerchantNetworkSubsystem へ Trust / StoryFlag を通知
// - FBXMerchantNetworkState の ActiveQuests / CompletedQuests / FailedQuests と同期
UCLASS()
class BX_API UBXQuestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Quest")
    TSoftObjectPtr<UDataTable> QuestDataTable;

    // --- 受注 ---

    // 受注可否を確認 (Trust / 前提依頼 / ストーリーフラグ)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Quest")
    bool CanAcceptQuest(FName QuestId) const;

    // 依頼を受注する。成功時 true を返す。
    UFUNCTION(BlueprintCallable, Category="BX|Quest")
    bool AcceptQuest(FName QuestId);

    // --- 進捗 ---

    // 進捗カウントを delta だけ加算し、達成チェックを行う
    UFUNCTION(BlueprintCallable, Category="BX|Quest")
    void AddQuestProgress(FName QuestId, int32 Delta = 1);

    // 補助タグを追加する (Discover / UseExtract 系)
    UFUNCTION(BlueprintCallable, Category="BX|Quest")
    void AddQuestProgressTag(FName QuestId, FGameplayTag Tag);

    // 任意の FailureRuleTag を持つ受注中依頼をすべて失敗にする
    UFUNCTION(BlueprintCallable, Category="BX|Quest")
    void TriggerFailureByTag(FGameplayTag FailureTag);

    // --- 状態照会 ---

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Quest")
    bool IsQuestActive(FName QuestId) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Quest")
    bool IsQuestCompleted(FName QuestId) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Quest")
    bool IsQuestFailed(FName QuestId) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Quest")
    FBXQuestRuntimeProgress GetQuestProgress(FName QuestId) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Quest")
    TArray<FName> GetActiveQuestIds() const;

    // 指定商人の受注可能依頼リストを返す
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Quest")
    TArray<FName> GetAvailableQuestIdsForMerchant(FName MerchantId) const;

    // --- SaveGame 連携 ---

    UFUNCTION(BlueprintCallable, Category="BX|Quest")
    void SaveQuestState(UPARAM(ref) FBXMerchantNetworkState& OutState) const;

    UFUNCTION(BlueprintCallable, Category="BX|Quest")
    void LoadQuestState(const FBXMerchantNetworkState& InState);

    UFUNCTION(BlueprintCallable, Category="BX|Quest")
    void ResetForNewProfile();

    // --- Delegates ---

    UPROPERTY(BlueprintAssignable, Category="BX|Quest")
    FOnQuestAccepted OnQuestAccepted;

    UPROPERTY(BlueprintAssignable, Category="BX|Quest")
    FOnQuestCompleted OnQuestCompleted;

    UPROPERTY(BlueprintAssignable, Category="BX|Quest")
    FOnQuestFailed OnQuestFailed;

    UPROPERTY(BlueprintAssignable, Category="BX|Quest")
    FOnQuestProgress OnQuestProgress;

private:
    TMap<FName, FBXQuestRuntimeProgress> ActiveProgress;
    TArray<FName> CompletedQuestIds;
    TArray<FName> FailedQuestIds;

    const FBXQuestTableRow* FindRow(FName QuestId) const;
    UBXMerchantNetworkSubsystem* GetMerchantSubsystem() const;
    void FinishQuest(FName QuestId);
    void DistributeRewards(const FBXQuestTableRow& Row);
};
