// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/BXEnums.h"

#include "FBXArmorItemRow.generated.h"

// DT_BX_ArmorItems の行構造体 — 防具アイテム定義 (Sprint 19)
USTRUCT(BlueprintType)
struct BX_API FBXArmorItemRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Armor")
    FText DisplayName;

    // DT_BX_Armor の RowName (防具クラスパラメータへの参照)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Armor")
    FName ArmorRowName = FName("armor_class_1");

    // このアイテムが保護する部位リスト
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Armor")
    TArray<EBXBodyPart> AreaCovered;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Armor", meta=(ClampMin="1.0"))
    float MaxDurability = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Armor", meta=(ClampMin="0.0"))
    float WeightKg = 1.0f;
};
