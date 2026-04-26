// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/MerchantHub/FBXMerchantHubViewModels.h"

#include "BXMerchantHubSubsystem.generated.h"

class UDataTable;
class UBXMerchantNetworkSubsystem;
class UBXQuestSubsystem;

// 商人ハブ ホーム画面状態集約サブシステム (SPEC §29-3)
// - 現在選択商人の集約 ViewModel を構築
// - 商人一覧行 ViewModel を構築
// - 最近の変動ログを管理
// - BP からクレジット / 地域 / 警告タグを設定
UCLASS()
class BX_API UBXMerchantHubSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // DT_BX_Merchants への参照 (UBXMerchantNetworkSubsystem.MerchantDataTable と同じアセット)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|UI|MerchantHub")
    TSoftObjectPtr<UDataTable> MerchantDataTable;

    // --- 選択状態 ---

    UFUNCTION(BlueprintCallable, Category="BX|UI|MerchantHub")
    void SelectMerchant(FName MerchantId);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|UI|MerchantHub")
    FName GetSelectedMerchantId() const { return SelectedMerchantId; }

    // --- ViewModel 構築 ---

    // 現在選択商人のホーム画面 ViewModel を構築する
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|UI|MerchantHub")
    FBXMerchantHubHomeViewModel BuildHomeViewModel() const;

    // 全商人の一覧行 ViewModel を構築する (Trust 昇順)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|UI|MerchantHub")
    TArray<FBXMerchantHubMerchantRowViewModel> BuildMerchantList() const;

    // --- 外部状態の設定 ---

    UFUNCTION(BlueprintCallable, Category="BX|UI|MerchantHub")
    void SetPlayerCredits(int32 Credits);

    UFUNCTION(BlueprintCallable, Category="BX|UI|MerchantHub")
    void SetCurrentRegion(FName RegionId);

    UFUNCTION(BlueprintCallable, Category="BX|UI|MerchantHub")
    void AddWarningTag(FGameplayTag Tag);

    UFUNCTION(BlueprintCallable, Category="BX|UI|MerchantHub")
    void ClearWarningTags();

    // ログ行を先頭に追加する (最大 5 行を保持)
    UFUNCTION(BlueprintCallable, Category="BX|UI|MerchantHub")
    void AddRecentLogLine(FText Line);

    UFUNCTION(BlueprintCallable, Category="BX|UI|MerchantHub")
    void ClearRecentLog();

private:
    FName SelectedMerchantId;
    int32 PlayerCredits = 0;
    FName CurrentRegionId;
    TArray<FText> RecentLogLines;
    FGameplayTagContainer WarningTags;

    static constexpr int32 MaxLogLines = 5;

    UBXMerchantNetworkSubsystem* GetMerchantNetworkSub() const;
    UBXQuestSubsystem* GetQuestSub() const;
};
