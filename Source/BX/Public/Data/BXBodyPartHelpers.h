// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/BXEnums.h"

// BoneName → EBXBodyPart 変換ユーティリティ (SPEC §14-3)
// UE5 SK_Mannequin 標準ボーン名に基づく簡易マッピング。
// ボーン階層精密判定は Sprint 将来対応。
struct BX_API FBXBodyPartHelpers
{
    static EBXBodyPart BoneNameToBodyPart(FName BoneName);
};
