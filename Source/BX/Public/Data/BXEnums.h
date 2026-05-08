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

// 武器カテゴリ (SPEC §14-4)
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

// 射撃モード (SPEC §14-4)
UENUM(BlueprintType)
enum class EBXFireMode : uint8
{
    SemiAuto    UMETA(DisplayName = "Semi Auto"),
    FullAuto    UMETA(DisplayName = "Full Auto"),
    Burst       UMETA(DisplayName = "Burst"),
    Pump        UMETA(DisplayName = "Pump"),
    Bolt        UMETA(DisplayName = "Bolt Action"),
};

// 被弾部位 — 7部位固定 (SPEC §14-5)
UENUM(BlueprintType)
enum class EBXBodyPart : uint8
{
    Head        UMETA(DisplayName = "Head"),
    Chest       UMETA(DisplayName = "Chest"),
    Abdomen     UMETA(DisplayName = "Abdomen"),
    LeftArm     UMETA(DisplayName = "Left Arm"),
    RightArm    UMETA(DisplayName = "Right Arm"),
    LeftLeg     UMETA(DisplayName = "Left Leg"),
    RightLeg    UMETA(DisplayName = "Right Leg"),
};

// 防具クラス (SPEC §14-5-1)
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

// ダメージ種別 (SPEC §14-5 / CLAUDE.md §8 GameplayTag 体系準拠)
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

// アイテムカテゴリ (SPEC §15-1)
UENUM(BlueprintType)
enum class EBXItemCategory : uint8
{
    Medical     UMETA(DisplayName = "Medical"),
    Ammo        UMETA(DisplayName = "Ammo"),
    Weapon      UMETA(DisplayName = "Weapon"),
    Armor       UMETA(DisplayName = "Armor"),
    Food        UMETA(DisplayName = "Food"),
    Collectible UMETA(DisplayName = "Collectible"),
    Key         UMETA(DisplayName = "Key"),
    Equipment   UMETA(DisplayName = "Equipment"),
    Quest       UMETA(DisplayName = "Quest"),
    Misc        UMETA(DisplayName = "Misc"),
};

// インベントリスロット種別 (SPEC §15-1)
UENUM(BlueprintType)
enum class EBXInventorySlotType : uint8
{
    PrimaryWeapon   UMETA(DisplayName = "Primary Weapon"),
    Sidearm         UMETA(DisplayName = "Sidearm"),
    Armor           UMETA(DisplayName = "Armor"),
    QuickUse        UMETA(DisplayName = "Quick Use"),
    Pouch           UMETA(DisplayName = "Pouch"),
    Backpack        UMETA(DisplayName = "Backpack"),
    Keyring         UMETA(DisplayName = "Keyring"),
};

// 状態異常分類 (SPEC §15-2)
UENUM(BlueprintType)
enum class EBXStatusType : uint8
{
    Bleed       UMETA(DisplayName = "Bleed"),
    Fracture    UMETA(DisplayName = "Fracture"),
    Pain        UMETA(DisplayName = "Pain"),
    Dehydration UMETA(DisplayName = "Dehydration"),
    Fatigue     UMETA(DisplayName = "Fatigue"),
    Neuro       UMETA(DisplayName = "Neuro Critical"),
};

// 抽出種別 (SPEC §18-2)
UENUM(BlueprintType)
enum class EBXExtractType : uint8
{
    Checkpoint  UMETA(DisplayName = "Checkpoint"),
    Rope        UMETA(DisplayName = "Rope"),
    Vehicle     UMETA(DisplayName = "Vehicle"),
    Hidden      UMETA(DisplayName = "Hidden"),
    Merchant    UMETA(DisplayName = "Merchant"),
};

// 抽出マーカー表示種別 (SPEC §18-2)
UENUM(BlueprintType)
enum class EBXExtractMarkerVisibility : uint8
{
    AlwaysVisible   UMETA(DisplayName = "Always Visible"),
    DiscoveredOnly  UMETA(DisplayName = "Discovered Only"),
    Conditional     UMETA(DisplayName = "Conditional"),
    Hidden          UMETA(DisplayName = "Hidden"),
};

// 商人種別 (SPEC §21-1)
UENUM(BlueprintType)
enum class EBXMerchantType : uint8
{
    Trader      UMETA(DisplayName = "Trader"),
    Medic       UMETA(DisplayName = "Medic"),
    Mechanic    UMETA(DisplayName = "Mechanic"),
    Courier     UMETA(DisplayName = "Courier"),
    Informant   UMETA(DisplayName = "Informant"),
};

// Trust 解放種別 (SPEC §21-1)
UENUM(BlueprintType)
enum class EBXMerchantRewardType : uint8
{
    ItemPool        UMETA(DisplayName = "Item Pool"),
    StorageSlots    UMETA(DisplayName = "Storage Slots"),
    RespawnNode     UMETA(DisplayName = "Respawn Node"),
    Service         UMETA(DisplayName = "Service"),
    Discount        UMETA(DisplayName = "Discount"),
    Quest           UMETA(DisplayName = "Quest"),
    Conversation    UMETA(DisplayName = "Conversation"),
    ExtractSupport  UMETA(DisplayName = "Extract Support"),
};

// 商人 Trust 段階 (SPEC §21-1 / SPEC_GAPS G-01)
UENUM(BlueprintType)
enum class EBXTrustLevel : uint8
{
    Level1 UMETA(DisplayName = "Level 1 - 不信"),
    Level2 UMETA(DisplayName = "Level 2 - 中立"),
    Level3 UMETA(DisplayName = "Level 3 - 信頼"),
    Level4 UMETA(DisplayName = "Level 4 - 盟友"),
};

// 依頼種別 (SPEC §21-2)
UENUM(BlueprintType)
enum class EBXQuestType : uint8
{
    Collect             UMETA(DisplayName = "Collect"),
    Deliver             UMETA(DisplayName = "Deliver"),
    Survive             UMETA(DisplayName = "Survive"),
    Extract             UMETA(DisplayName = "Extract"),
    Discover            UMETA(DisplayName = "Discover"),
    Kill                UMETA(DisplayName = "Kill"),
    UseExtract          UMETA(DisplayName = "Use Extract"),
    TurnInCollectorItem UMETA(DisplayName = "Turn In Collector Item"),
};

// 取引画面タブ (SPEC §29-4)
UENUM(BlueprintType)
enum class EBXTradeTab : uint8
{
    Buy     UMETA(DisplayName = "Buy"),
    Sell    UMETA(DisplayName = "Sell"),
    Compare UMETA(DisplayName = "Compare"),
};

// 医療アイテム種別 (SPEC §15-3)
UENUM(BlueprintType)
enum class EBXMedicalType : uint8
{
    Bandage     UMETA(DisplayName = "Bandage"),
    Tourniquet  UMETA(DisplayName = "Tourniquet"),
    TraumaKit   UMETA(DisplayName = "Trauma Kit"),
    Splint      UMETA(DisplayName = "Splint"),
    Painkiller  UMETA(DisplayName = "Painkiller"),
    Hydration   UMETA(DisplayName = "Hydration"),
    Surgery     UMETA(DisplayName = "Surgery"),
    NeuroSaver  UMETA(DisplayName = "Neuro Saver"),
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

// 視点モード (SPEC §23-4)
UENUM(BlueprintType)
enum class EBXViewMode : uint8
{
    First UMETA(DisplayName = "First Person"),
    Third UMETA(DisplayName = "Third Person"),
};

// 移動状態 (SPEC §23-4)
UENUM(BlueprintType)
enum class EBXLocomotionState : uint8
{
    Idle   UMETA(DisplayName = "Idle"),
    Walk   UMETA(DisplayName = "Walk"),
    Run    UMETA(DisplayName = "Run"),
    Sprint UMETA(DisplayName = "Sprint"),
    Crouch UMETA(DisplayName = "Crouch"),
    Prone  UMETA(DisplayName = "Prone"),
};

// リーン状態 (SPEC §23-4)
UENUM(BlueprintType)
enum class EBXLeanState : uint8
{
    None  UMETA(DisplayName = "None"),
    Left  UMETA(DisplayName = "Left"),
    Right UMETA(DisplayName = "Right"),
};

// 戦闘状態 (SPEC §23-4)
UENUM(BlueprintType)
enum class EBXCombatState : uint8
{
    None      UMETA(DisplayName = "None"),
    Aiming    UMETA(DisplayName = "Aiming"),
    Firing    UMETA(DisplayName = "Firing"),
    Reloading UMETA(DisplayName = "Reloading"),
    Switching UMETA(DisplayName = "Switching"),
};

// 武器スロット (SPEC §23-4 / §16-4)
UENUM(BlueprintType)
enum class EBXWeaponSlot : uint8
{
    Primary   UMETA(DisplayName = "Primary"),
    Secondary UMETA(DisplayName = "Secondary"),
    Pistol    UMETA(DisplayName = "Pistol"),
    Melee     UMETA(DisplayName = "Melee"),
};

// 重量状態 (SPEC §23-4)
UENUM(BlueprintType)
enum class EBXCarryState : uint8
{
    Normal     UMETA(DisplayName = "Normal"),
    Heavy      UMETA(DisplayName = "Heavy"),
    Overweight UMETA(DisplayName = "Overweight"),
};

// 表示言語 (SPEC §23-4)
UENUM(BlueprintType)
enum class EBXLanguage : uint8
{
    Japanese UMETA(DisplayName = "Japanese"),
    English  UMETA(DisplayName = "English"),
};

// 敵 AI 状態 (Sprint 24)
UENUM(BlueprintType)
enum class EBXEnemyState : uint8
{
    Idle    UMETA(DisplayName = "Idle"),
    Patrol  UMETA(DisplayName = "Patrol"),
    Alert   UMETA(DisplayName = "Alert"),
    Combat  UMETA(DisplayName = "Combat"),
    Dead    UMETA(DisplayName = "Dead"),
};

// ドローン AI 状態 (Sprint 26)
UENUM(BlueprintType)
enum class EBXDroneState : uint8
{
    Idle    UMETA(DisplayName = "Idle"),
    Patrol  UMETA(DisplayName = "Patrol"),
    Track   UMETA(DisplayName = "Track"),
    Attack  UMETA(DisplayName = "Attack"),
    Dead    UMETA(DisplayName = "Dead"),
};

// スポーンポイント種別 (Sprint 25)
UENUM(BlueprintType)
enum class EBXSpawnType : uint8
{
    Default    UMETA(DisplayName = "Default"),
    Extraction UMETA(DisplayName = "Extraction"),
    Tactical   UMETA(DisplayName = "Tactical"),
    Hidden     UMETA(DisplayName = "Hidden"),
};

// ロケーション難易度 (Sprint 25)
UENUM(BlueprintType)
enum class EBXLocationDifficulty : uint8
{
    Easy    UMETA(DisplayName = "Easy"),
    Normal  UMETA(DisplayName = "Normal"),
    Hard    UMETA(DisplayName = "Hard"),
    Extreme UMETA(DisplayName = "Extreme"),
};

// インタラクト対象種別 (SPEC §23-7)
UENUM(BlueprintType)
enum class EBXInteractTargetType : uint8
{
    ExtractPoint    UMETA(DisplayName = "Extract Point"),
    LootContainer   UMETA(DisplayName = "Loot Container"),
    CorpseLoot      UMETA(DisplayName = "Corpse Loot"),
    Door            UMETA(DisplayName = "Door"),
    Switch          UMETA(DisplayName = "Switch"),
    Terminal        UMETA(DisplayName = "Terminal"),
    QuestObject     UMETA(DisplayName = "Quest Object"),
    Pickup          UMETA(DisplayName = "Pickup"),
    Generic         UMETA(DisplayName = "Generic"),
};
