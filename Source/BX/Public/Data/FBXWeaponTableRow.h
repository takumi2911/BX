// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Data/BXEnums.h"

class USkeletalMesh;
class USoundBase;
class UNiagaraSystem;
class UAnimMontage;
class UCurveFloat;
class UTexture2D;

#include "FBXWeaponTableRow.generated.h"

// DT_BX_Weapons の行構造体 (SPEC §14-4)
USTRUCT(BlueprintType)
struct BX_API FBXWeaponTableRow : public FTableRowBase
{
    GENERATED_BODY()

    // --- 識別 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon")
    FName WeaponId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon")
    EBXWeaponCategory Category = EBXWeaponCategory::AR;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon")
    FName Caliber;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon")
    FName AmmoId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon")
    TArray<EBXFireMode> FireModes;

    // --- 射撃性能 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    float BaseDamage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    float MuzzleVelocity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    int32 RPM = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    float VerticalRecoil = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    float HorizontalRecoil = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    float Ergonomics = 0.0f;

    // --- マガジン / リロード ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    int32 DefaultMagSize = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    float ReloadTimeSec = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    float TacticalReloadTimeSec = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    float ADSTimeSec = 0.0f;

    // --- 物理 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    float WeightKg = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Stats")
    float DurabilityMax = 100.0f;

    // --- 経済 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Economy")
    int32 BasePriceCredits = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Economy", meta=(ClampMin="0.0", ClampMax="1.0"))
    float PriceVarianceRatio = 0.0f;

    // --- アタッチメント ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Attachments")
    FGameplayTagContainer AllowedAttachments;

    // --- アセット参照 (非同期ロード前提) ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<USkeletalMesh> WeaponMesh1P;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<USkeletalMesh> WeaponMesh3P;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<USoundBase> FireSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<UNiagaraSystem> MuzzleFlashFX;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<UAnimMontage> ReloadMontage1P;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<UAnimMontage> EquipMontage1P;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<UCurveFloat> AimOffsetCurve;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<UTexture2D> IconTexture;

    // --- UI / メタ ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Economy")
    FGameplayTag SellCategoryTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Loot")
    float SpawnWeight = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Economy")
    int32 MinMerchantTier = 1;

    // --- ストーリーロック ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Story")
    bool bIsStoryLocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon|Story")
    FName RequiredStoryFlag;
};
