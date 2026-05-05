// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/BXEnums.h"

// BoneName → EBXBodyPart 変換 / 部位ダメージマルチプライヤー (SPEC §14-3, §14-5)
struct BX_API FBXBodyPartHelpers
{
    static EBXBodyPart BoneNameToBodyPart(FName BoneName);

    // SPEC §14-5 部位ダメージマルチプライヤー (Head=4.0 致命傷、Leg=0.82 四肢)
    static float GetBodyPartDamageMultiplier(EBXBodyPart Part);
};
