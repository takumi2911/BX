// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/FBXArmorTableRow.h"
#include "Data/FBXArmorItemRow.h"
#include "Data/BXEnums.h"

class UDataTable;

#include "AC_BX_ArmorEquipment.generated.h"

// 防具装備コンポーネント (SPEC §14-5)
// Sprint 17: Chest のみ / Sprint 19: 全 7 部位 + アイテム装備 API
UCLASS(ClassGroup=(BX), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class BX_API UAC_BX_ArmorEquipment : public UActorComponent
{
    GENERATED_BODY()

public:
    UAC_BX_ArmorEquipment();

    // DT_BX_Armor への参照 (防具クラスパラメータ)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BX|Armor")
    TObjectPtr<UDataTable> ArmorDataTable;

    // DT_BX_ArmorItems への参照 (アイテム定義) — Sprint 19
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BX|Armor")
    TObjectPtr<UDataTable> ArmorItemDataTable;

    // 全 7 部位の装備スロット
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Armor")
    FBXEquippedArmor HeadArmor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Armor")
    FBXEquippedArmor ChestArmor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Armor")
    FBXEquippedArmor AbdomenArmor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Armor")
    FBXEquippedArmor LeftArmArmor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Armor")
    FBXEquippedArmor RightArmArmor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Armor")
    FBXEquippedArmor LeftLegArmor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Armor")
    FBXEquippedArmor RightLegArmor;

    // その部位に防具があるか
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

    // アイテムを装備する — AreaCovered の全部位にスロットをセット — Sprint 19
    UFUNCTION(BlueprintCallable, Category="BX|Armor")
    void EquipArmorItem(FName ItemRowName);

private:
    FBXEquippedArmor* GetEquippedArmorForPart(EBXBodyPart Part);
    const FBXEquippedArmor* GetEquippedArmorForPart(EBXBodyPart Part) const;
};
