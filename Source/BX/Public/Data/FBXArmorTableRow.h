// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "FBXArmorTableRow.generated.h"

// DT_BX_Armor の行構造体 — 防具クラスパラメータ (SPEC §14-5)
USTRUCT(BlueprintType)
struct BX_API FBXArmorClassRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Armor")
    int32 ArmorClass = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Armor")
    float BasePenetrationThreshold = 10.0f;

    // 非貫通時に透過するダメージ割合 (SPEC §14-5-8)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Armor", meta=(ClampMin="0.0", ClampMax="1.0"))
    float BluntThroughputRatio = 0.28f;

    // 弾薬 ArmorDamageRatio との乗数 (SPEC §14-5-9)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Armor")
    float DurabilityDamageMultiplier = 1.0f;

    // 修理時に失われる耐久割合 (Sprint 18 以降)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Armor", meta=(ClampMin="0.0", ClampMax="1.0"))
    float RepairLossRatio = 0.10f;
};

// 装備中防具インスタンス (Sprint 17: Chest のみ使用)
USTRUCT(BlueprintType)
struct BX_API FBXEquippedArmor
{
    GENERATED_BODY()

    // DT_BX_Armor の RowName (例: "armor_class_3")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Armor")
    FName ArmorRowName = FName("armor_class_3");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Armor", meta=(ClampMin="0.0"))
    float CurrentDurability = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Armor", meta=(ClampMin="1.0"))
    float MaxDurability = 100.0f;
};
