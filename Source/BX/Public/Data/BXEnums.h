// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BXEnums.generated.h"

// ゲームモード (SPEC §4 / SPEC_GAPS G-01)
UENUM(BlueprintType)
enum class EBXGameMode : uint8
{
    HardcoreReal        UMETA(DisplayName = "リアルハードコア"),
    HardcoreMerchant    UMETA(DisplayName = "商人預け保持ハードコア"),
};

// 武器カテゴリ (SPEC_GAPS G-02)
UENUM(BlueprintType)
enum class EBXWeaponCategory : uint8
{
    HG   UMETA(DisplayName = "Handgun"),
    SMG  UMETA(DisplayName = "SMG"),
    AR   UMETA(DisplayName = "Assault Rifle"),
    SG   UMETA(DisplayName = "Shotgun"),
    DMR  UMETA(DisplayName = "DMR"),
    SR   UMETA(DisplayName = "Sniper Rifle"),
};

// 射撃モード (SPEC_GAPS G-02)
UENUM(BlueprintType)
enum class EBXFireMode : uint8
{
    SemiAuto    UMETA(DisplayName = "Semi Auto"),
    FullAuto    UMETA(DisplayName = "Full Auto"),
    Burst       UMETA(DisplayName = "Burst"),
    Pump        UMETA(DisplayName = "Pump"),
    Bolt        UMETA(DisplayName = "Bolt Action"),
};

// 被弾部位 (SPEC_GAPS G-03)
UENUM(BlueprintType)
enum class EBXBodyPart : uint8
{
    Head        UMETA(DisplayName = "Head"),
    Chest       UMETA(DisplayName = "Chest"),
    Stomach     UMETA(DisplayName = "Stomach"),
    LeftArm     UMETA(DisplayName = "Left Arm"),
    RightArm    UMETA(DisplayName = "Right Arm"),
    LeftLeg     UMETA(DisplayName = "Left Leg"),
    RightLeg    UMETA(DisplayName = "Right Leg"),
};

// 防具クラス (SPEC_GAPS G-03)
UENUM(BlueprintType)
enum class EBXArmorClass : uint8
{
    Class1 UMETA(DisplayName = "Class 1"),
    Class2 UMETA(DisplayName = "Class 2"),
    Class3 UMETA(DisplayName = "Class 3"),
    Class4 UMETA(DisplayName = "Class 4"),
    Class5 UMETA(DisplayName = "Class 5"),
    Class6 UMETA(DisplayName = "Class 6"),
};

// ダメージ種別 (CLAUDE.md §8 GameplayTag 体系準拠)
UENUM(BlueprintType)
enum class EBXDamageType : uint8
{
    Bullet      UMETA(DisplayName = "Bullet"),
    Blunt       UMETA(DisplayName = "Blunt"),
    Explosive   UMETA(DisplayName = "Explosive"),
    Slash       UMETA(DisplayName = "Slash"),
    Fall        UMETA(DisplayName = "Fall"),
    Fire        UMETA(DisplayName = "Fire"),
};

// 商人 Trust 段階 (SPEC_GAPS G-01: TrustLevel 1〜4)
UENUM(BlueprintType)
enum class EBXTrustLevel : uint8
{
    Level1 UMETA(DisplayName = "Level 1 - 不信"),
    Level2 UMETA(DisplayName = "Level 2 - 中立"),
    Level3 UMETA(DisplayName = "Level 3 - 信頼"),
    Level4 UMETA(DisplayName = "Level 4 - 盟友"),
};

// 依頼種別 (SPEC_GAPS G-05)
UENUM(BlueprintType)
enum class EBXQuestType : uint8
{
    Delivery        UMETA(DisplayName = "納品"),
    Recovery        UMETA(DisplayName = "回収"),
    Investigation   UMETA(DisplayName = "調査"),
    Elimination     UMETA(DisplayName = "排除"),
    Transport       UMETA(DisplayName = "搬送"),
    Liberation      UMETA(DisplayName = "解放"),
    Special         UMETA(DisplayName = "特殊"),
};

// リスポーンノード種別 (SPEC_GAPS G-06)
UENUM(BlueprintType)
enum class EBXRespawnNodeType : uint8
{
    Hideout     UMETA(DisplayName = "隠れ家"),
    House       UMETA(DisplayName = "民家"),
    Clinic      UMETA(DisplayName = "診療所"),
    Warehouse   UMETA(DisplayName = "倉庫"),
    Underground UMETA(DisplayName = "地下避難室"),
};
