// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/BXEnums.h"

#include "BXSaveManager.generated.h"

class USaveGame_BX;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBXSaveComplete, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBXLoadComplete, bool, bSuccess, USaveGame_BX*, SaveGame);

// セーブ管理サブシステム (Sprint 21 / SPEC §16-3)
// UGameplayStatics::SaveGameToSlot / LoadGameFromSlot のラッパ
UCLASS()
class BX_API UBXSaveManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="BX|Save")
    void SaveGame(const FString& SlotSuffix = TEXT("slot0"));

    UFUNCTION(BlueprintCallable, Category="BX|Save")
    void LoadGame(const FString& SlotSuffix = TEXT("slot0"));

    // 新規ゲーム作成 (保存はしない - SaveGame() を明示的に呼ぶこと)
    UFUNCTION(BlueprintCallable, Category="BX|Save")
    void NewGame(const FString& ProfileName, EBXGameMode GameMode);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Save")
    USaveGame_BX* GetCurrentSave() const { return CurrentSave; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Save")
    bool HasActiveSave() const { return CurrentSave != nullptr; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Save")
    bool DoesSaveExist(const FString& SlotSuffix = TEXT("slot0")) const;

    // レイド終了時の自動セーブ (TotalRaids++ してから保存)
    UFUNCTION(BlueprintCallable, Category="BX|Save")
    void AutoSaveOnRaidEnd();

    // 中断スナップショットをクリアしてから保存 (抽出成功時)
    UFUNCTION(BlueprintCallable, Category="BX|Save")
    void SaveOnExtractSuccess();

    UPROPERTY(BlueprintAssignable, Category="BX|Save")
    FOnBXSaveComplete OnSaveComplete;

    UPROPERTY(BlueprintAssignable, Category="BX|Save")
    FOnBXLoadComplete OnLoadComplete;

private:
    UPROPERTY()
    TObjectPtr<USaveGame_BX> CurrentSave;

    FString GetSlotName(const FString& SlotSuffix) const;
};
