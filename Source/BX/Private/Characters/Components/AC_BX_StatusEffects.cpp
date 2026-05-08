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

    if (NeuroCriticalCountdown > 0.0f)
    {
        NeuroCriticalCountdown -= DeltaTime;
        if (NeuroCriticalCountdown <= 0.0f)
        {
            NeuroCriticalCountdown = 0.0f;
            RemoveStatus(FName("status_neuro_critical"));
        }
    }

    for (int32 i = ActiveStatuses.Num() - 1; i >= 0; --i)
    {
        FBXActiveStatus& Status = ActiveStatuses[i];
        const FBXStatusEffectTableRow* Row = FindRow(Status.StatusId);
        if (!Row) { ActiveStatuses.RemoveAt(i); continue; }

        if (Row->bUsesDuration)
        {
            Status.RemainingDurationSec -= DeltaTime;
            if (Status.RemainingDurationSec <= 0.0f)
            {
                ActiveStatuses.RemoveAt(i);
                continue;
            }
        }

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

    if (Row->StatusType == EBXStatusType::Neuro)
    {
        if (bNeuroCriticalUsed) { return; }
        bNeuroCriticalUsed = true;
        NeuroCriticalCountdown = Row->BaseDurationSec;
    }

    if (Row->StatusType == EBXStatusType::Bleed)
    {
        EvictSmallBleedIfLargeBleed(StatusId);
    }

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
    NewStatus.StatusId              = StatusId;
    NewStatus.StatusType            = Row->StatusType;
    NewStatus.RemainingDurationSec  = Row->bUsesDuration ? Row->BaseDurationSec : -1.0f;
    NewStatus.StackCount            = 1;
    NewStatus.NextTickCountdown     = Row->TickIntervalSec;
    ActiveStatuses.Add(NewStatus);
}

bool UAC_BX_StatusEffects::TreatStatus(FName StatusId, FName MedItemId)
{
    const FBXStatusEffectTableRow* Row = FindRow(StatusId);
    if (!Row) { return false; }
    if (Row->RequiredMedItemId != NAME_None && Row->RequiredMedItemId != MedItemId)
    {
        return false;
    }
    RemoveStatus(StatusId);
    if (Row->StatusType == EBXStatusType::Neuro) { NeuroCriticalCountdown = 0.0f; }
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

float UAC_BX_StatusEffects::GetCombinedAimStabilityRatio() const
{
    return CombineRatios([](const FBXStatusEffectTableRow& Row) { return Row.AimStabilityRatio; });
}

float UAC_BX_StatusEffects::GetCombinedADSSpeedRatio() const
{
    return CombineRatios([](const FBXStatusEffectTableRow& Row) { return Row.ADSSpeedRatio; });
}

float UAC_BX_StatusEffects::GetCombinedStaminaRecoveryRatio() const
{
    return CombineRatios([](const FBXStatusEffectTableRow& Row) { return Row.StaminaRecoveryRatio; });
}

TArray<EBXStatusType> UAC_BX_StatusEffects::GetActiveStatusTypes() const
{
    TArray<EBXStatusType> Result;
    for (const FBXActiveStatus& S : ActiveStatuses)
    {
        Result.AddUnique(S.StatusType);
    }
    return Result;
}

TArray<FName> UAC_BX_StatusEffects::GetActiveStatusIds() const
{
    TArray<FName> Result;
    for (const FBXActiveStatus& S : ActiveStatuses)
    {
        Result.Add(S.StatusId);
    }
    return Result;
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
    // DamagePerTick の適用はオーナーの HealthComponent に委ねる (BP 側でバインド)
    // BP は GetCombinedMoveSpeedRatio 等を Tick で参照して各ステータスを更新する
    if (Row.DamagePerTick <= 0.0f) { return; }
    (void)Status;
}

void UAC_BX_StatusEffects::EvictSmallBleedIfLargeBleed(FName IncomingStatusId)
{
    if (IncomingStatusId != FName("status_large_bleed")) { return; }
    RemoveStatus(FName("status_small_bleed"));
}

float UAC_BX_StatusEffects::CombineRatios(
    TFunctionRef<float(const FBXStatusEffectTableRow&)> Getter) const
{
    float Combined = 1.0f;
    for (const FBXActiveStatus& S : ActiveStatuses)
    {
        const FBXStatusEffectTableRow* Row = FindRow(S.StatusId);
        if (Row)
        {
            float RatioPerStack = Getter(*Row);
            for (int32 i = 0; i < S.StackCount; ++i)
            {
                Combined *= RatioPerStack;
            }
        }
    }
    return Combined;
}
