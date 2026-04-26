// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Systems/Medical/BXMedicalUseSubsystem.h"
#include "Data/Medical/FBXMedicalItemRow.h"
#include "Characters/Components/AC_BX_StatusEffects.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UBXMedicalUseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UBXMedicalUseSubsystem::Deinitialize()
{
    CancelCurrentUse();
    Super::Deinitialize();
}

bool UBXMedicalUseSubsystem::UseItem(FName MedicalItemId, UAC_BX_StatusEffects* StatusComp)
{
    if (!CanUseItem(MedicalItemId)) { return false; }

    const FBXMedicalItemRow* Row = FindRow(MedicalItemId);
    if (!Row) { return false; }

    UWorld* World = GetGameInstance()->GetWorld();
    if (!World) { return false; }

    CurrentItemId = MedicalItemId;
    CurrentStatusComp = StatusComp;
    UseStartTimeSec = World->GetTimeSeconds();
    CurrentUseTimeSec = Row->UseTimeSec;

    if (Row->UseTimeSec <= 0.0f)
    {
        OnUseComplete();
    }
    else
    {
        World->GetTimerManager().SetTimer(
            UseCompleteTimer,
            this,
            &UBXMedicalUseSubsystem::OnUseComplete,
            Row->UseTimeSec,
            false
        );
    }

    return true;
}

void UBXMedicalUseSubsystem::CancelCurrentUse()
{
    if (CurrentItemId == NAME_None) { return; }

    UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
    if (World)
    {
        World->GetTimerManager().ClearTimer(UseCompleteTimer);
    }
    CurrentItemId = NAME_None;
    CurrentStatusComp = nullptr;
    UseStartTimeSec = 0.0f;
    CurrentUseTimeSec = 0.0f;
}

bool UBXMedicalUseSubsystem::CanUseItem(FName MedicalItemId) const
{
    if (IsCurrentlyUsing()) { return false; }
    if (IsOnCooldown(MedicalItemId)) { return false; }
    if (FindRow(MedicalItemId) == nullptr) { return false; }
    return true;
}

bool UBXMedicalUseSubsystem::IsCurrentlyUsing() const
{
    return CurrentItemId != NAME_None;
}

float UBXMedicalUseSubsystem::GetUseProgressRatio() const
{
    if (!IsCurrentlyUsing() || CurrentUseTimeSec <= 0.0f) { return 0.0f; }
    UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
    if (!World) { return 0.0f; }
    float Elapsed = World->GetTimeSeconds() - UseStartTimeSec;
    return FMath::Clamp(Elapsed / CurrentUseTimeSec, 0.0f, 1.0f);
}

FName UBXMedicalUseSubsystem::GetCurrentUsingItemId() const
{
    return CurrentItemId;
}

bool UBXMedicalUseSubsystem::IsOnCooldown(FName MedicalItemId) const
{
    return CooldownTimers.Contains(MedicalItemId);
}

float UBXMedicalUseSubsystem::GetCooldownRemainingRatio(FName MedicalItemId) const
{
    const FTimerHandle* Handle = CooldownTimers.Find(MedicalItemId);
    if (!Handle) { return 0.0f; }

    UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
    if (!World) { return 0.0f; }

    const FBXMedicalItemRow* Row = FindRow(MedicalItemId);
    if (!Row || Row->CooldownSec <= 0.0f) { return 0.0f; }

    float Remaining = World->GetTimerManager().GetTimerRemaining(*Handle);
    return FMath::Clamp(Remaining / Row->CooldownSec, 0.0f, 1.0f);
}

void UBXMedicalUseSubsystem::NotifyDamageReceived()
{
    if (!IsCurrentlyUsing()) { return; }
    const FBXMedicalItemRow* Row = FindRow(CurrentItemId);
    if (Row && Row->bCancelOnDamage) { CancelCurrentUse(); }
}

void UBXMedicalUseSubsystem::NotifySprintStarted()
{
    if (!IsCurrentlyUsing()) { return; }
    const FBXMedicalItemRow* Row = FindRow(CurrentItemId);
    if (Row && Row->bCancelOnSprint) { CancelCurrentUse(); }
}

void UBXMedicalUseSubsystem::NotifyWeaponFired()
{
    if (!IsCurrentlyUsing()) { return; }
    const FBXMedicalItemRow* Row = FindRow(CurrentItemId);
    if (Row && Row->bCancelOnWeaponUse) { CancelCurrentUse(); }
}

void UBXMedicalUseSubsystem::ResetForNewRaid()
{
    CancelCurrentUse();
    UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
    if (World)
    {
        for (auto& Pair : CooldownTimers)
        {
            World->GetTimerManager().ClearTimer(Pair.Value);
        }
    }
    CooldownTimers.Empty();
}

void UBXMedicalUseSubsystem::OnUseComplete()
{
    if (CurrentItemId == NAME_None) { return; }

    const FBXMedicalItemRow* Row = FindRow(CurrentItemId);
    FName CompletedItemId = CurrentItemId;

    // 効果を StatusEffects コンポーネントへ適用
    if (Row && IsValid(CurrentStatusComp))
    {
        // 状態異常を解除
        for (FName StatusId : Row->CuresStatusIds)
        {
            CurrentStatusComp->TreatStatus(StatusId, CompletedItemId);
        }

        // Neuro Critical 解除
        if (Row->bCanResolveNeuroCritical && CurrentStatusComp->IsNeuroCriticalActive())
        {
            CurrentStatusComp->TreatStatus(FName("neuro_critical"), CompletedItemId);
        }

        // バフ / デバフ付与
        if (Row->AppliedBuffStatusId != NAME_None)
        {
            CurrentStatusComp->ApplyStatus(Row->AppliedBuffStatusId);
        }
    }

    // クールダウン開始
    if (Row && Row->CooldownSec > 0.0f)
    {
        StartCooldown(CompletedItemId, Row->CooldownSec);
    }

    // 使用状態リセット
    CurrentItemId = NAME_None;
    CurrentStatusComp = nullptr;
    UseStartTimeSec = 0.0f;
    CurrentUseTimeSec = 0.0f;
}

const FBXMedicalItemRow* UBXMedicalUseSubsystem::FindRow(FName MedicalItemId) const
{
    UDataTable* Table = MedicalItemDataTable.Get();
    if (!Table) { return nullptr; }
    return Table->FindRow<FBXMedicalItemRow>(MedicalItemId, TEXT("BXMedicalUseSubsystem"));
}

void UBXMedicalUseSubsystem::StartCooldown(FName MedicalItemId, float CooldownSec)
{
    UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
    if (!World) { return; }

    FTimerHandle Handle;
    World->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this, MedicalItemId]()
    {
        CooldownTimers.Remove(MedicalItemId);
    }), CooldownSec, false);

    CooldownTimers.Add(MedicalItemId, Handle);
}
