// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Characters/Components/AC_BX_WeaponHandler.h"
#include "Characters/APlayerCharacterBase.h"
#include "Data/FBXWeaponTableRow.h"
#include "Engine/DataTable.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UAC_BX_WeaponHandler::UAC_BX_WeaponHandler()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAC_BX_WeaponHandler::BeginPlay()
{
    Super::BeginPlay();
    OwnerPlayer = Cast<APlayerCharacterBase>(GetOwner());
    UE_LOG(LogTemp, Log, TEXT("UAC_BX_WeaponHandler::BeginPlay — owner=%s"),
        OwnerPlayer.IsValid() ? *OwnerPlayer->GetName() : TEXT("NULL"));
}

void UAC_BX_WeaponHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (FireCooldownRemaining > 0.0f)
    {
        FireCooldownRemaining = FMath::Max(0.0f, FireCooldownRemaining - DeltaTime);
    }
}

// =========================================================
// 武器管理
// =========================================================

void UAC_BX_WeaponHandler::EquipWeapon(EBXWeaponSlot Slot, FName WeaponRowName)
{
    UE_LOG(LogTemp, Warning, TEXT("UAC_BX_WeaponHandler::EquipWeapon — Slot=%d Row=%s"),
        static_cast<int32>(Slot), *WeaponRowName.ToString());

    EquippedWeaponRowNames.Add(Slot, WeaponRowName);

    if (IsValid(WeaponDataTable))
    {
        const FBXWeaponTableRow* Row = WeaponDataTable->FindRow<FBXWeaponTableRow>(
            WeaponRowName, TEXT("UAC_BX_WeaponHandler::EquipWeapon"));
        if (Row)
        {
            CurrentMagazineAmmo.Add(Slot, Row->DefaultMagSize);
            UE_LOG(LogTemp, Warning, TEXT("EquipWeapon: [%s] equipped — mag=%d RPM=%d dmg=%.1f"),
                *Row->DisplayName.ToString(), Row->DefaultMagSize, Row->RPM, Row->BaseDamage);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("EquipWeapon: Row '%s' が DT_BX_Weapons に見つかりません — Row Name を確認してください"),
                *WeaponRowName.ToString());
            CurrentMagazineAmmo.Add(Slot, 0);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("EquipWeapon: WeaponDataTable が NULL — BP_BX_Player > WeaponHandlerComponent > Class Defaults で DT_BX_Weapons をアサインしてください"));
        CurrentMagazineAmmo.Add(Slot, 0);
    }

    SwitchWeaponSlot(Slot);
}

void UAC_BX_WeaponHandler::SwitchWeaponSlot(EBXWeaponSlot NewSlot)
{
    UE_LOG(LogTemp, Warning, TEXT("UAC_BX_WeaponHandler::SwitchWeaponSlot — %d → %d"),
        static_cast<int32>(CurrentSlot), static_cast<int32>(NewSlot));
    CurrentSlot = NewSlot;
    BP_OnWeaponSlotChanged(NewSlot);
}

const FBXWeaponTableRow* UAC_BX_WeaponHandler::GetCurrentWeaponRow() const
{
    if (!IsValid(WeaponDataTable)) { return nullptr; }
    const FName* RowNamePtr = EquippedWeaponRowNames.Find(CurrentSlot);
    if (!RowNamePtr || *RowNamePtr == NAME_None) { return nullptr; }
    return WeaponDataTable->FindRow<FBXWeaponTableRow>(*RowNamePtr, TEXT("UAC_BX_WeaponHandler::GetCurrentWeaponRow"));
}

// =========================================================
// 発砲
// =========================================================

void UAC_BX_WeaponHandler::FirePrimary()
{
    UE_LOG(LogTemp, Warning, TEXT("UAC_BX_WeaponHandler::FirePrimary CALLED — Slot=%d"), static_cast<int32>(CurrentSlot));

    if (FireCooldownRemaining > 0.0f)
    {
        UE_LOG(LogTemp, Verbose, TEXT("FirePrimary: cooldown=%.3f"), FireCooldownRemaining);
        return;
    }
    if (bIsReloading)
    {
        UE_LOG(LogTemp, Log, TEXT("FirePrimary: リロード中"));
        return;
    }

    const FBXWeaponTableRow* Row = GetCurrentWeaponRow();
    if (!Row)
    {
        UE_LOG(LogTemp, Warning, TEXT("FirePrimary: 現在スロットに武器が装備されていません"));
        return;
    }

    int32& Ammo = CurrentMagazineAmmo.FindOrAdd(CurrentSlot);
    if (Ammo <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("FirePrimary: 弾切れ → Reload 開始"));
        Reload();
        return;
    }

    --Ammo;
    FireCooldownRemaining = (Row->RPM > 0) ? (60.0f / static_cast<float>(Row->RPM)) : 0.1f;

    UE_LOG(LogTemp, Warning, TEXT("FirePrimary: 発砲 [%s] 残弾=%d/%d cooldown=%.3f"),
        *EquippedWeaponRowNames.FindRef(CurrentSlot).ToString(),
        Ammo, Row->DefaultMagSize, FireCooldownRemaining);

    PerformFireTrace(Row->BaseDamage);
}

void UAC_BX_WeaponHandler::PerformFireTrace(float BaseDamage)
{
    APlayerCharacterBase* Player = OwnerPlayer.Get();
    if (!IsValid(Player)) { return; }

    UCameraComponent* Cam = Player->CameraFirstPerson;
    if (!IsValid(Cam)) { return; }

    const FVector Start  = Cam->GetComponentLocation();
    const FVector End    = Start + Cam->GetForwardVector() * TraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params(TEXT("WeaponTrace"), false, Player);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        AActor* HitActor = Hit.GetActor();
        UE_LOG(LogTemp, Warning, TEXT("FirePrimary: HIT → %s dist=%.1fcm dmg=%.1f"),
            HitActor ? *HitActor->GetName() : TEXT("NULL"),
            Hit.Distance, BaseDamage);

        if (IsValid(HitActor) && IsValid(Hit.GetComponent()))
        {
            const FVector ShotDir = (End - Start).GetSafeNormal();
            UGameplayStatics::ApplyPointDamage(
                HitActor, BaseDamage, ShotDir, Hit,
                Player->GetInstigatorController(),
                Player,
                nullptr);  // DamageType: Sprint 15 で BXDamageType に変更
        }
    }
    else
    {
        UE_LOG(LogTemp, Verbose, TEXT("FirePrimary: no hit"));
    }
}

// =========================================================
// リロード
// =========================================================

void UAC_BX_WeaponHandler::Reload()
{
    UE_LOG(LogTemp, Warning, TEXT("UAC_BX_WeaponHandler::Reload CALLED"));

    if (bIsReloading)
    {
        UE_LOG(LogTemp, Log, TEXT("Reload: already reloading"));
        return;
    }

    const FBXWeaponTableRow* Row = GetCurrentWeaponRow();
    if (!Row)
    {
        UE_LOG(LogTemp, Warning, TEXT("Reload: 武器が装備されていません"));
        return;
    }

    const int32 CurrentAmmo = CurrentMagazineAmmo.FindRef(CurrentSlot);
    if (CurrentAmmo >= Row->DefaultMagSize)
    {
        UE_LOG(LogTemp, Log, TEXT("Reload: フルマガジン — リロード不要"));
        return;
    }

    bIsReloading = true;
    const float ReloadTime = (CurrentAmmo > 0) ? Row->TacticalReloadTimeSec : Row->ReloadTimeSec;
    UE_LOG(LogTemp, Warning, TEXT("Reload: 開始 (%.2fs) ammo=%d/%d"),
        ReloadTime, CurrentAmmo, Row->DefaultMagSize);

    GetWorld()->GetTimerManager().SetTimer(
        ReloadTimerHandle,
        FTimerDelegate::CreateUObject(this, &UAC_BX_WeaponHandler::OnReloadComplete),
        ReloadTime, false);
}

void UAC_BX_WeaponHandler::OnReloadComplete()
{
    bIsReloading = false;
    const FBXWeaponTableRow* Row = GetCurrentWeaponRow();
    const int32 MagSize = Row ? Row->DefaultMagSize : 0;
    CurrentMagazineAmmo.Add(CurrentSlot, MagSize);
    UE_LOG(LogTemp, Warning, TEXT("Reload: 完了 — ammo=%d"), MagSize);
    BP_OnReloadComplete(MagSize);
}
