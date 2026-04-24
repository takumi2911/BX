// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Characters/Components/AC_BX_StatusEffects.h"
#include "Data/FBXStatusEffectTableRow.h"
#include "Engine/DataTable.h"

UAC_BX_StatusEffects::UAC_BX_StatusEffects()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAC_BX_StatusEffects::BeginPlay()
{
    Super::BeginPlay();
}

void UAC_BX_StatusEffects::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Neuro Critical カウントダウン
    if (NeuroCriticalCountdown > 0.0f)
    {
        NeuroCriticalCountdown -= DeltaTime;
        if (NeuroCriticalCountdown <= 0.0f)
        {
            NeuroCriticalCountdown = 0.0f;
            // 時間切れ — 即死ロジックは BP/GameMode 側で HasStatusOfType(Neuro) を監視して実装
            RemoveStatus(FName("neuro_critical"));
        }
    }

    // 各アクティブ状態異常の Tick 処理
    for (int32 i = ActiveStatuses.Num() - 1; i >= 0; --i)
    {
        FBXActiveStatus& Status = ActiveStatuses[i];
        const FBXStatusEffectTableRow* Row = FindRow(Status.StatusId);
        if (!Row) { ActiveStatuses.RemoveAt(i); continue; }

        // 持続時間カウントダウン
        if (Row->bUsesDuration)
        {
            Status.RemainingDurationSec -= DeltaTime;
            if (Status.RemainingDurationSec <= 0.0f)
            {
                ActiveStatuses.RemoveAt(i);
                continue;
            }
        }

        // Tick 間隔でダメージ等を適用
        if (Row->TickIntervalSec > 0.0f)
        {
            Status.NextTickCountdown -= DeltaTime;
            if (Status.NextTickCountdown <= 0.0f)
            {
                Status.NextTickCountdown = Row->TickIntervalSec;
                ApplyTickEffect(*Row, Status);
            }
        }
    }
}

void UAC_BX_StatusEffects::ApplyStatus(FName StatusId)
{
    const FBXStatusEffectTableRow* Row = FindRow(StatusId);
    if (!Row) { return; }

    // Neuro Critical は 1 レイド 1 回限り
    if (Row->StatusType == EBXStatusType::Neuro)
    {
        if (bNeuroCriticalUsed) { return; }
        bNeuroCriticalUsed = true;
        NeuroCriticalCountdown = Row->BaseDurationSec;
    }

    // 大出血は小出血を上書き除去
    if (Row->StatusType == EBXStatusType::Bleed)
    {
        EvictSmallBleedIfLargeBleed(StatusId);
    }

    // スタック処理
    if (Row->bCanStack)
    {
        for (FBXActiveStatus& Existing : ActiveStatuses)
        {
            if (Existing.StatusId == StatusId)
            {
                Existing.StackCount = FMath::Min(Existing.StackCount + 1, Row->MaxStackCount);
                if (Row->bUsesDuration) { Existing.RemainingDurationSec = Row->BaseDurationSec; }
                return;
            }
        }
    }
    else
    {
        // 非スタック: 既に存在する場合は持続時間を更新するだけ
        for (FBXActiveStatus& Existing : ActiveStatuses)
        {
            if (Existing.StatusId == StatusId)
            {
                if (Row->bUsesDuration) { Existing.RemainingDurationSec = Row->BaseDurationSec; }
                return;
            }
        }
    }

    FBXActiveStatus NewStatus;
    NewStatus.StatusId = StatusId;
    NewStatus.StatusType = Row->StatusType;
    NewStatus.RemainingDurationSec = Row->bUsesDuration ? Row->BaseDurationSec : -1.0f;
    NewStatus.StackCount = 1;
    NewStatus.NextTickCountdown = Row->TickIntervalSec;
    ActiveStatuses.Add(NewStatus);
}

bool UAC_BX_StatusEffects::TreatStatus(FName StatusId, FName MedItemId)
{
    const FBXStatusEffectTableRow* Row = FindRow(StatusId);
    if (!Row) { return false; }

    // 必要な治療アイテムが一致するか確認
    if (Row->RequiredMedItemId != NAME_None && Row->RequiredMedItemId != MedItemId)
    {
        return false;
    }

    RemoveStatus(StatusId);
    if (Row->StatusType == EBXStatusType::Neuro)
    {
        NeuroCriticalCountdown = 0.0f;
    }
    return true;
}

void UAC_BX_StatusEffects::RemoveStatus(FName StatusId)
{
    ActiveStatuses.RemoveAll([&StatusId](const FBXActiveStatus& S)
    {
        return S.StatusId == StatusId;
    });
}

bool UAC_BX_StatusEffects::HasStatusOfType(EBXStatusType Type) const
{
    for (const FBXActiveStatus& S : ActiveStatuses)
    {
        if (S.StatusType == Type) { return true; }
    }
    return false;
}

float UAC_BX_StatusEffects::GetCombinedMoveSpeedRatio() const
{
    return CombineRatios([](const FBXStatusEffectTableRow& Row) { return Row.MoveSpeedRatio; });
}

bool UAC_BX_StatusEffects::IsNeuroCriticalActive() const
{
    return NeuroCriticalCountdown > 0.0f;
}

void UAC_BX_StatusEffects::ResetForNewRaid()
{
    ActiveStatuses.Empty();
    bNeuroCriticalUsed = false;
    NeuroCriticalCountdown = 0.0f;
}

const FBXStatusEffectTableRow* UAC_BX_StatusEffects::FindRow(FName StatusId) const
{
    UDataTable* Table = StatusEffectDataTable.Get();
    if (!Table) { return nullptr; }
    return Table->FindRow<FBXStatusEffectTableRow>(StatusId, TEXT("AC_BX_StatusEffects"));
}

void UAC_BX_StatusEffects::ApplyTickEffect(const FBXStatusEffectTableRow& Row, FBXActiveStatus& Status)
{
    if (Row.DamagePerTick <= 0.0f) { return; }

    // ダメージ適用はオーナーの HealthComponent に委ねる (BP 側でバインド)
    // ここでは Event を発火させる代わりに直接 HealthComponent を取得する設計を採らず、
    // BP がこのコンポーネントの Tick を監視して DamagePerTick 分のダメージを与える想定。
    // C++ 側で疎結合を保つため、OnStatusTickDamage デリゲートを将来拡張で追加できる。
    (void)Status;
}

void UAC_BX_StatusEffects::EvictSmallBleedIfLargeBleed(FName IncomingStatusId)
{
    // "large_bleed" のみが "small_bleed" を除去する
    if (IncomingStatusId != FName("large_bleed")) { return; }
    RemoveStatus(FName("small_bleed"));
}

float UAC_BX_StatusEffects::CombineRatios(TFunctionRef<float(const FBXStatusEffectTableRow&)> Getter) const
{
    float Combined = 1.0f;
    for (const FBXActiveStatus& S : ActiveStatuses)
    {
        const FBXStatusEffectTableRow* Row = FindRow(S.StatusId);
        if (Row)
        {
            // スタックがある場合は効果を乗算で累積 (StackCount 回適用)
            float RatioPerStack = Getter(*Row);
            for (int32 i = 0; i < S.StackCount; ++i)
            {
                Combined *= RatioPerStack;
            }
        }
    }
    return Combined;
}
