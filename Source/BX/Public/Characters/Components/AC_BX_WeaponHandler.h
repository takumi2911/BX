// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/BXEnums.h"

class APlayerCharacterBase;
class UDataTable;
struct FBXWeaponTableRow;

#include "AC_BX_WeaponHandler.generated.h"

// 武器ハンドラーコンポーネント (SPEC §14-4 / §23-5)
// 武器スロット管理・発砲(LineTrace)・リロード・スロット切替を担う。
// VFX / SE / 貫通計算は Sprint 15 以降。
UCLASS(ClassGroup=(BX), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class BX_API UAC_BX_WeaponHandler : public UActorComponent
{
    GENERATED_BODY()

public:
    UAC_BX_WeaponHandler();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // DT_BX_Weapons への参照 (BP_BX_Player の WeaponHandlerComponent → Class Defaults でアサイン)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BX|Weapon")
    TObjectPtr<UDataTable> WeaponDataTable;

    // DT_BX_Ammo への参照 (BP_BX_Player の WeaponHandlerComponent → Class Defaults でアサイン)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BX|Weapon")
    TObjectPtr<UDataTable> AmmoDataTable;

    // 発砲 LineTrace 距離 (cm)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BX|Weapon", meta=(ClampMin="100.0"))
    float TraceDistance = 5000.0f;

    // 現在装備中スロット
    UPROPERTY(BlueprintReadOnly, Category="BX|Weapon")
    EBXWeaponSlot CurrentSlot = EBXWeaponSlot::Primary;

    // 各スロットの装備武器 RowName (DT_BX_Weapons の Row Name)
    UPROPERTY(BlueprintReadOnly, Category="BX|Weapon")
    TMap<EBXWeaponSlot, FName> EquippedWeaponRowNames;

    // 各スロットの現在装弾数
    UPROPERTY(BlueprintReadOnly, Category="BX|Weapon")
    TMap<EBXWeaponSlot, int32> CurrentMagazineAmmo;

    // リロード中フラグ
    UPROPERTY(BlueprintReadOnly, Category="BX|Weapon")
    bool bIsReloading = false;

    // 連射クールダウン残り (秒)
    UPROPERTY(BlueprintReadOnly, Category="BX|Weapon")
    float FireCooldownRemaining = 0.0f;

    // スロットに武器をセットし装弾数を初期化する
    UFUNCTION(BlueprintCallable, Category="BX|Weapon")
    void EquipWeapon(EBXWeaponSlot Slot, FName WeaponRowName);

    // 現在スロットを切り替える
    UFUNCTION(BlueprintCallable, Category="BX|Weapon")
    void SwitchWeaponSlot(EBXWeaponSlot NewSlot);

    // 現在装備中武器で 1 発撃つ (クールダウン・装弾数・LineTrace)
    UFUNCTION(BlueprintCallable, Category="BX|Weapon")
    void FirePrimary();

    // リロード開始 (DT の ReloadTimeSec 後に装弾数をマガジン上限まで戻す)
    UFUNCTION(BlueprintCallable, Category="BX|Weapon")
    void Reload();

    // 現在装備中武器の DT 行を取得 (C++ 専用: UFUNCTION は raw struct ポインタ返し不可)
    const FBXWeaponTableRow* GetCurrentWeaponRow() const;

    // --- BlueprintImplementableEvent ---

    UFUNCTION(BlueprintImplementableEvent, Category="BX|Weapon")
    void BP_OnWeaponSlotChanged(EBXWeaponSlot NewSlot);

    UFUNCTION(BlueprintImplementableEvent, Category="BX|Weapon")
    void BP_OnReloadComplete(int32 NewAmmoCount);

private:
    TWeakObjectPtr<APlayerCharacterBase> OwnerPlayer;
    FTimerHandle ReloadTimerHandle;

    void PerformFireTrace(float BaseDamage);
    void OnReloadComplete();
};
