// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Characters/Components/AC_BX_WeaponHandler.h"
#include "Characters/APlayerCharacterBase.h"
#include "Characters/Components/AC_BX_HealthBodyParts.h"
#include "Characters/Components/AC_BX_ArmorEquipment.h"
#include "Data/FBXWeaponTableRow.h"
#include "Data/FBXAmmoTableRow.h"
#include "Data/FBXArmorTableRow.h"
#include "Data/BXBodyPartHelpers.h"
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

int32 UAC_BX_WeaponHandler::GetCurrentMagazineAmmo() const
{
    const int32* AmmoPtr = CurrentMagazineAmmo.Find(CurrentSlot);
    return AmmoPtr ? *AmmoPtr : 0;
}

int32 UAC_BX_WeaponHandler::GetMagazineCapacity() const
{
    const FBXWeaponTableRow* Row = GetCurrentWeaponRow();
    return Row ? Row->DefaultMagSize : 0;
}

FName UAC_BX_WeaponHandler::GetCurrentWeaponRowName() const
{
    const FName* RowName = EquippedWeaponRowNames.Find(CurrentSlot);
    return RowName ? *RowName : NAME_None;
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

    // 弾薬テーブルから BaseDamage + AmmoRow を取得 (案A: WeaponRow.AmmoId → AmmoDataTable)
    const FBXAmmoTableRow* AmmoRow = nullptr;
    float BaseDamage = Row->BaseDamage;
    if (IsValid(AmmoDataTable) && Row->AmmoId != NAME_None)
    {
        AmmoRow = AmmoDataTable->FindRow<FBXAmmoTableRow>(
            Row->AmmoId, TEXT("UAC_BX_WeaponHandler::FirePrimary"));
        if (AmmoRow)
        {
            BaseDamage = AmmoRow->Damage;
        }
        else
        {
            UE_LOG(LogTemp, Warning,
                TEXT("FirePrimary: AmmoRow '%s' が DT_BX_Ammo に見つかりません — WeaponRow.BaseDamage=%.1f をフォールバック使用"),
                *Row->AmmoId.ToString(), Row->BaseDamage);
        }
    }
    else if (!IsValid(AmmoDataTable))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("FirePrimary: AmmoDataTable が NULL — BP_BX_Player > WeaponHandlerComponent > Class Defaults で DT_BX_Ammo をアサインしてください"));
    }

    UE_LOG(LogTemp, Warning, TEXT("FirePrimary: 発砲 [%s] 残弾=%d/%d cooldown=%.3f BaseDmg=%.1f"),
        *EquippedWeaponRowNames.FindRef(CurrentSlot).ToString(),
        Ammo, Row->DefaultMagSize, FireCooldownRemaining, BaseDamage);

    PerformFireTrace(BaseDamage, AmmoRow);
}

void UAC_BX_WeaponHandler::PerformFireTrace(float BaseDamage, const FBXAmmoTableRow* AmmoRow)
{
    APlayerCharacterBase* Player = OwnerPlayer.Get();
    if (!IsValid(Player)) { return; }

    UCameraComponent* Cam = Player->CameraFirstPerson;
    if (!IsValid(Cam)) { return; }

    const FVector Start = Cam->GetComponentLocation();
    const FVector End   = Start + Cam->GetForwardVector() * TraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params(TEXT("WeaponTrace"), true, Player);  // bTraceComplex=true でボーン名取得
    Params.bReturnPhysicalMaterial = true;

    if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        UE_LOG(LogTemp, Verbose, TEXT("FirePrimary: no hit"));
        return;
    }

    AActor* HitActor = Hit.GetActor();
    const FName BoneName = Hit.BoneName;

    UAC_BX_HealthBodyParts* HealthComp = IsValid(HitActor)
        ? HitActor->FindComponentByClass<UAC_BX_HealthBodyParts>()
        : nullptr;

    if (!IsValid(HealthComp))
    {
        // HealthBodyParts を持たない Actor は ApplyPointDamage にフォールバック
        UE_LOG(LogTemp, Warning, TEXT("FirePrimary: HIT → %s dist=%.1fcm dmg=%.1f (no HealthBodyParts)"),
            HitActor ? *HitActor->GetName() : TEXT("NULL"), Hit.Distance, BaseDamage);

        if (IsValid(HitActor) && IsValid(Hit.GetComponent()))
        {
            const FVector ShotDir = (End - Start).GetSafeNormal();
            UGameplayStatics::ApplyPointDamage(HitActor, BaseDamage, ShotDir, Hit,
                Player->GetInstigatorController(), Player, nullptr);
        }
        return;
    }

    // --- 部位判定 ---
    const EBXBodyPart Part         = FBXBodyPartHelpers::BoneNameToBodyPart(BoneName);
    const float BodyPartMult       = FBXBodyPartHelpers::GetBodyPartDamageMultiplier(Part);
    const UEnum* PartEnum          = StaticEnum<EBXBodyPart>();
    const FString PartName         = PartEnum
        ? PartEnum->GetNameStringByValue(static_cast<int64>(Part)) : TEXT("?");

    // --- 防具コンポーネント取得 ---
    UAC_BX_ArmorEquipment* ArmorComp = HitActor->FindComponentByClass<UAC_BX_ArmorEquipment>();

    float FinalDamage;

    if (IsValid(ArmorComp) && ArmorComp->HasArmor(Part) && AmmoRow != nullptr)
    {
        // ========== SPEC §14-5-4〜§14-5-9 防具貫通計算 ==========

        const FBXArmorClassRow ArmorClass  = ArmorComp->GetArmorClassRow(Part);
        const float CurrentDur             = ArmorComp->GetCurrentDurability(Part);
        const float MaxDur                 = ArmorComp->GetMaxDurability(Part);

        // §14-5-4 実効防具閾値
        const float DurabilityRatio        = FMath::Clamp(CurrentDur / FMath::Max(1.0f, MaxDur), 0.0f, 1.0f);
        const float EffectivePenThresh     = ArmorClass.BasePenetrationThreshold * FMath::Lerp(0.55f, 1.0f, DurabilityRatio);

        // §14-5-5 貫通スコア
        const float PenScore               = AmmoRow->Penetration - EffectivePenThresh;

        // §14-5-6 貫通確率
        float PenChance;
        if      (PenScore <= -15.0f) { PenChance = 0.0f; }
        else if (PenScore >=  15.0f) { PenChance = 1.0f; }
        else                         { PenChance = 0.5f + (PenScore / 30.0f); }

        // ランダムロール
        const bool bPenetrated = FMath::FRand() < PenChance;

        if (bPenetrated)
        {
            // §14-5-7 貫通成功時ダメージ
            const float Falloff = FMath::Clamp(
                1.0f - FMath::Max(0.0f, EffectivePenThresh - AmmoRow->Penetration) * 0.01f,
                0.70f, 1.00f);
            FinalDamage = BaseDamage * BodyPartMult * Falloff;
            UE_LOG(LogTemp, Warning,
                TEXT("FirePrimary HIT: Part=%s Penetrated! BaseDmg=%.1f Mult=%.2f Falloff=%.2f FinalDmg=%.1f | Pen=%.1f EffThresh=%.1f PenScore=%.2f Chance=%.0f%% Dur=%.1f (Actor=%s)"),
                *PartName, BaseDamage, BodyPartMult, Falloff, FinalDamage,
                AmmoRow->Penetration, EffectivePenThresh, PenScore, PenChance * 100.0f,
                CurrentDur, *HitActor->GetName());
        }
        else
        {
            // §14-5-8 非貫通(Blunt)ダメージ
            FinalDamage = BaseDamage * BodyPartMult * ArmorClass.BluntThroughputRatio;
            UE_LOG(LogTemp, Warning,
                TEXT("FirePrimary HIT: Part=%s Blunt! BaseDmg=%.1f Mult=%.2f BluntRatio=%.2f FinalDmg=%.1f | Pen=%.1f EffThresh=%.1f PenScore=%.2f Chance=%.0f%% Dur=%.1f (Actor=%s)"),
                *PartName, BaseDamage, BodyPartMult, ArmorClass.BluntThroughputRatio, FinalDamage,
                AmmoRow->Penetration, EffectivePenThresh, PenScore, PenChance * 100.0f,
                CurrentDur, *HitActor->GetName());
        }

        // §14-5-9 防具耐久減少
        const float DurabilityDmg = BaseDamage * AmmoRow->ArmorDamageRatio * ArmorClass.DurabilityDamageMultiplier;
        ArmorComp->ApplyDurabilityDamage(Part, DurabilityDmg);
    }
    else
    {
        // 防具なし (Sprint 16 ロジック)
        FinalDamage = BaseDamage * BodyPartMult;
        UE_LOG(LogTemp, Warning,
            TEXT("FirePrimary HIT: Part=%s, BaseDmg=%.1f, Mult=%.2f, FinalDmg=%.1f (no armor) (Actor=%s Bone=%s)"),
            *PartName, BaseDamage, BodyPartMult, FinalDamage, *HitActor->GetName(), *BoneName.ToString());
    }

    HealthComp->ApplyDamageToBodyPart(Part, FinalDamage);
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
