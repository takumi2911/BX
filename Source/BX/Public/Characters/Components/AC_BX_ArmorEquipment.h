// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/FBXArmorTableRow.h"
#include "Data/BXEnums.h"

class UDataTable;

#include "AC_BX_ArmorEquipment.generated.h"

// 防具装備コンポーネント (SPEC §14-5)
// Sprint 17: Chest のみ対応。Sprint 18 で Head / Abdomen / 四肢を追加。
UCLASS(ClassGroup=(BX), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class BX_API UAC_BX_ArmorEquipment : public UActorComponent
{
    GENERATED_BODY()

public:
    UAC_BX_ArmorEquipment();

    // DT_BX_Armor への参照 (BP の Class Defaults でアサイン)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BX|Armor")
    TObjectPtr<UDataTable> ArmorDataTable;

    // Sprint 17: Chest 防具のみ実装
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Armor")
    FBXEquippedArmor ChestArmor;

    // その部位に防具があるか (Sprint 17 は Chest のみ true)
    UFUNCTION(BlueprintCallable, Category="BX|Armor")
    bool HasArmor(EBXBodyPart Part) const;

    // 部位の FBXArmorClassRow を取得 (DT 参照、なければデフォルト値)
    UFUNCTION(BlueprintCallable, Category="BX|Armor")
    FBXArmorClassRow GetArmorClassRow(EBXBodyPart Part) const;

    // 部位の現在耐久度を取得
    UFUNCTION(BlueprintCallable, Category="BX|Armor")
    float GetCurrentDurability(EBXBodyPart Part) const;

    // 部位の最大耐久度を取得
    UFUNCTION(BlueprintCallable, Category="BX|Armor")
    float GetMaxDurability(EBXBodyPart Part) const;

    // 防具耐久度にダメージを適用 (clamp 0 〜 MaxDurability)
    UFUNCTION(BlueprintCallable, Category="BX|Armor")
    void ApplyDurabilityDamage(EBXBodyPart Part, float DurabilityDamage);

private:
    FBXEquippedArmor* GetEquippedArmorForPart(EBXBodyPart Part);
    const FBXEquippedArmor* GetEquippedArmorForPart(EBXBodyPart Part) const;
};
