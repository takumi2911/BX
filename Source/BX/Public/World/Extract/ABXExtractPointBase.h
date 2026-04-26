// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "World/Extract/AC_BX_ExtractEvaluator.h"

#include "ABXExtractPointBase.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UAudioComponent;

// 抽出イベント通知データ (SPEC §18-3-3)
USTRUCT(BlueprintType)
struct BX_API FBXExtractEventData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    FName ExtractId;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    FName RegionId;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    bool bWasSuccessful = false;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    float StartTimeSec = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    float EndTimeSec = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    FGameplayTag CancelReasonTag;

    UPROPERTY(BlueprintReadOnly, Category="BX|Extract")
    FName TriggeredWaveId;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExtractEvent, const FBXExtractEventData&, EventData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExtractProgressChanged, float, ProgressRatio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExtractConditionsChanged, const FBXExtractEvalResult&, EvalResult);

// 抽出ポイント基底 Actor (SPEC §18-3)
// Blueprint 派生で見た目・演出を差し替えること
// UCLASS に Blueprintable を付けているため BP_BX_ExtractPoint_xxx のベースとして使用可
UCLASS(BlueprintType, Blueprintable, Abstract)
class BX_API ABXExtractPointBase : public AActor
{
    GENERATED_BODY()

public:
    ABXExtractPointBase();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // --- 設定 ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Extract")
    FName ExtractId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UAC_BX_ExtractEvaluator> ExtractEvaluator;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UBoxComponent> TriggerVolume;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UStaticMeshComponent> VisualMesh;

    // --- ランタイム状態クエリ ---

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Extract")
    float GetProgressRatio() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Extract")
    bool IsExtractionRunning() const { return bIsExtractionRunning; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Extract")
    bool IsPlayerInsideTrigger() const { return bIsPlayerInsideTrigger; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Extract")
    bool IsExtractionLocked() const { return bIsExtractionLocked; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Extract")
    FBXExtractEvalResult GetLastEvalResult() const { return LastEvalResult; }

    // --- 操作 ---

    // プレイヤーが条件を満たした上で抽出開始入力を行ったとき BP から呼ぶ
    UFUNCTION(BlueprintCallable, Category="BX|Extract")
    void StartExtraction();

    // --- 中断通知 (BP から被弾 / ADS / アイテム使用時に呼ぶ) ---

    UFUNCTION(BlueprintCallable, Category="BX|Extract")
    void NotifyDamageReceived();

    UFUNCTION(BlueprintCallable, Category="BX|Extract")
    void NotifyAimStarted();

    UFUNCTION(BlueprintCallable, Category="BX|Extract")
    void NotifyItemUsed();

    // --- Delegates ---

    UPROPERTY(BlueprintAssignable, Category="BX|Extract")
    FOnExtractEvent OnExtractEvent;

    UPROPERTY(BlueprintAssignable, Category="BX|Extract")
    FOnExtractProgressChanged OnProgressChanged;

    UPROPERTY(BlueprintAssignable, Category="BX|Extract")
    FOnExtractConditionsChanged OnConditionsChanged;

protected:
    UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // 条件再評価 → 変化があれば OnConditionsChanged を発火
    UFUNCTION(BlueprintCallable, Category="BX|Extract")
    void ReevaluateConditions();

    // Blueprint で追加コンポーネント / 演出を初期化する場合にオーバーライド
    UFUNCTION(BlueprintImplementableEvent, Category="BX|Extract")
    void BP_OnExtractionStarted();

    UFUNCTION(BlueprintImplementableEvent, Category="BX|Extract")
    void BP_OnExtractionCompleted();

    UFUNCTION(BlueprintImplementableEvent, Category="BX|Extract")
    void BP_OnExtractionCancelled(FGameplayTag CancelReason);

private:
    bool bIsPlayerInsideTrigger = false;
    bool bIsExtractionRunning   = false;
    bool bIsExtractionLocked    = false;
    bool bHasSatisfiedConditions = false;
    float ExtractionStartTimeSec = 0.0f;
    float ExtractionCountdownSec = 0.0f;
    FBXExtractEvalResult LastEvalResult;

    FTimerHandle ExtractionTimer;

    void OnExtractionComplete();
    void CancelExtraction(FGameplayTag Reason);
    void BroadcastEvent(bool bSuccess, FGameplayTag CancelReason, FName WaveId);
};
