#pragma once
// BXMatchSubsystem - レイド試合フロー管理 (Sprint 29)
// GameInstance Subsystem として常駐。状態機械で試合の流れを制御。

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BXMatchSubsystem.generated.h"

UENUM(BlueprintType)
enum class EBXMatchState : uint8
{
    Idle,        // ロビー待機
    PreGame,     // カウントダウン (3-2-1)
    InMatch,     // レイド中
    Extracting,  // 抽出チャネル中
    PostMatch,   // 試合終了 → 結果画面
};

USTRUCT(BlueprintType)
struct FBXRaidResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite) bool   bExtracted     = false;
    UPROPERTY(BlueprintReadWrite) float  SurvivalTime   = 0.f;
    UPROPERTY(BlueprintReadWrite) int32  EnemiesKilled  = 0;
    UPROPERTY(BlueprintReadWrite) float  LootValue      = 0.f;
    UPROPERTY(BlueprintReadWrite) FName  ExtractPointID;
    UPROPERTY(BlueprintReadWrite) FText  ResultMessage;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnMatchStateChanged, EBXMatchState, OldState, EBXMatchState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnMatchEnded, const FBXRaidResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnCountdownTick, int32, SecondsRemaining);

UCLASS()
class BX_API UBXMatchSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // --- 状態遷移 ---

    UFUNCTION(BlueprintCallable, Category="BX|Match")
    void StartPreGame(int32 CountdownSecs = 3);

    UFUNCTION(BlueprintCallable, Category="BX|Match")
    void StartMatch();

    UFUNCTION(BlueprintCallable, Category="BX|Match")
    void BeginExtraction(FName ExtractPointID);

    UFUNCTION(BlueprintCallable, Category="BX|Match")
    void CancelExtraction();

    UFUNCTION(BlueprintCallable, Category="BX|Match")
    void EndMatch(bool bExtracted, FName ExtractPointID = NAME_None);

    UFUNCTION(BlueprintCallable, Category="BX|Match")
    void ReturnToLobby();

    // --- クエリ ---

    UFUNCTION(BlueprintPure, Category="BX|Match")
    EBXMatchState GetMatchState() const { return MatchState; }

    UFUNCTION(BlueprintPure, Category="BX|Match")
    bool IsInMatch() const { return MatchState == EBXMatchState::InMatch; }

    UFUNCTION(BlueprintPure, Category="BX|Match")
    float GetMatchElapsedTime() const;

    UFUNCTION(BlueprintPure, Category="BX|Match")
    FBXRaidResult GetLastResult() const { return LastResult; }

    // --- 試合中のイベント集計 ---

    UFUNCTION(BlueprintCallable, Category="BX|Match")
    void NotifyEnemyKilled();

    UFUNCTION(BlueprintCallable, Category="BX|Match")
    void NotifyItemLooted(float Value);

    // --- Delegates ---

    UPROPERTY(BlueprintAssignable, Category="BX|Match")
    FOnMatchStateChanged OnMatchStateChanged;

    UPROPERTY(BlueprintAssignable, Category="BX|Match")
    FOnMatchEnded OnMatchEnded;

    UPROPERTY(BlueprintAssignable, Category="BX|Match")
    FOnCountdownTick OnCountdownTick;

private:
    EBXMatchState MatchState = EBXMatchState::Idle;
    FBXRaidResult LastResult;
    FBXRaidResult CurrentResult;
    float MatchStartTime  = 0.f;
    float ExtractionStart = 0.f;
    FName ActiveExtractID;

    // カウントダウン
    int32 CountdownRemaining = 0;
    FTimerHandle CountdownTimer;
    FTimerHandle ExtractTimer;

    void SetMatchState(EBXMatchState NewState);
    void TickCountdown();
    void OnExtractionComplete();
};
