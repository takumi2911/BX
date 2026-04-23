// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "FBXAmmoTableRow.generated.h"

// DT_BX_Ammo の行構造体 — 弾薬弾道パラメータ (SPEC §14-5-2)
USTRUCT(BlueprintType)
struct BX_API FBXAmmoTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Ammo")
    FName AmmoId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Ammo")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Ammo")
    FName Caliber;

    // --- 弾道 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Ammo|Ballistics")
    float Penetration = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Ammo|Ballistics")
    float Damage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Ammo|Ballistics")
    float ArmorDamageRatio = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Ammo|Ballistics", meta=(ClampMin="0.0", ClampMax="1.0"))
    float FragmentChance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Ammo|Ballistics")
    float VelocityFactor = 1.0f;
};
