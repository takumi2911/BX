# Part B. Claude Code 用実装仕様

このファイルは、ユーザー指定の「ChatGPT 向け 仕様書執筆ルール(BLACKOUT EXFIL)」に準拠して作成した、Claude Code 渡し用の統合 Markdown である。  
既存の実装優先章を、章番号順・表形式・受け入れ条件付きでまとめている。

## 収録章
- §14-4 武器 DataTable の行構造詳細
- §14-5 ダメージ計算モデル
- §15-1 アイテム DataTable とインベントリ基本構造
- §15-2 状態異常 DataTable と Neuro Critical 基本定義
- §15-3 医療アイテム詳細テーブル
- §16-3 セーブ構造体フィールド詳細
- §18-2 抽出条件データ構造
- §18-3 抽出ポイント Actor 基底仕様
- §21-1 商人 DataTable / Trust 解放構造
- §21-2 依頼 DataTable と進行構造
- §29-3 商人ハブのホーム画面 UI
- §29-4 商人ハブの取引画面

---

> 形式はユーザー指定の「ChatGPT 向け 仕様書執筆ルール(BLACKOUT EXFIL)」に準拠する。  
> 注記: 部位ダメージは **7部位**（Head / Chest / Abdomen / LeftArm / RightArm / LeftLeg / RightLeg）前提で記述する。  
> 注記: `§16-3` は依頼文に「6ブロック」とあるが 7 構造体が列挙されているため、**列挙された 7 構造体すべて**を定義する。

---

## §14-4 武器 DataTable の行構造詳細

### 概要
`DT_BX_Weapons` は、武器本体のゲームプレイ値・価格・許可アタッチメント・アセット参照を一元管理する武器マスタ DataTable である。  
Claude Code はこの章を元に `FBXWeaponTableRow` を `FTableRowBase` 派生で実装し、CSV からインポート可能な行構造を生成する。

### 仕様
- DataTable 名は `DT_BX_Weapons` とする。
- 行構造体名は `FBXWeaponTableRow` とする。
- 主キーは `WeaponId` とし、重複を禁止する。
- 価格は **代表値 + 変動率** で管理する。UI 表示価格は `BasePriceCredits * (1 ± PriceVarianceRatio)` を用いる。
- 武器カテゴリは `ENUM_BX_WeaponCategory`（HG / SMG / AR / SG / DMR / SR）で管理する。
- 射撃モードは `TArray<ENUM_BX_FireMode>` で管理する。
- 許可アタッチメントは `FGameplayTagContainer` を用いる。
- アセット参照は `TSoftObjectPtr<>` を用い、ロードは非同期を前提とする。
- 1P と 3P 表示の差分に対応するため、メッシュ参照は 1P / 3P を分ける。
- 発砲時の音・FX は武器固有参照とし、未設定時はカテゴリ既定値へフォールバック可能とする（実装は §14-4 準拠、既定値テーブルは別章追加時に定義）。
- `Caliber` は文字列ではなく `FName` 管理とし、弾薬テーブル（将来章）と `AmmoId` で接続する。
- `AllowedAttachments` にはスロット種別とカテゴリタグの両方を含めてよい。例: `Attachment.Muzzle.AR`, `Attachment.Sight.Rail`, `Attachment.Mag.AR556`.
- 耐久は武器破損状態・精度劣化・売却価格低下に利用する。
- `PriceVarianceRatio` は 0.00〜1.00 を想定する。

### データ構造

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `WeaponId` | `FName` | ✅ | 主キー。例: `ar_556a_01` |
| `DisplayName` | `FText` | ✅ | ゲーム内表示名 |
| `Category` | `ENUM_BX_WeaponCategory` | ✅ | 武器カテゴリ（HG / SMG / AR / SG / DMR / SR） |
| `Caliber` | `FName` | ✅ | 口径識別子。例: `9mm`, `556`, `762`, `12G`, `308` |
| `AmmoId` | `FName` | ✅ | 使用弾薬 ID。`DT_BX_Ammo` の主キーを参照 |
| `FireModes` | `TArray<ENUM_BX_FireMode>` | ✅ | 対応射撃モード配列 |
| `BaseDamage` | `float` | ✅ | 基本ダメージ（HP 単位） |
| `MuzzleVelocity` | `float` | ✅ | 初速（m/s） |
| `RPM` | `int32` | ✅ | 発射レート（rounds per minute） |
| `VerticalRecoil` | `float` | ✅ | 縦反動係数（無次元係数） |
| `HorizontalRecoil` | `float` | ✅ | 横反動係数（無次元係数） |
| `Ergonomics` | `float` | ✅ | 取り回し値。高いほど操作しやすい |
| `DefaultMagSize` | `int32` | ✅ | 標準マガジン装弾数（発） |
| `ReloadTimeSec` | `float` | ✅ | 空マガジンからの通常リロード時間（秒） |
| `TacticalReloadTimeSec` | `float` | ✅ | 残弾ありタクティカルリロード時間（秒） |
| `ADSTimeSec` | `float` | ✅ | 腰だめから ADS へ移行する時間（秒） |
| `WeightKg` | `float` | ✅ | 武器重量（kg） |
| `DurabilityMax` | `float` | ✅ | 最大耐久値 |
| `BasePriceCredits` | `int32` | ✅ | 基本価格（弾価値クレジット換算） |
| `PriceVarianceRatio` | `float` | ✅ | 価格変動率（0.00〜1.00） |
| `AllowedAttachments` | `FGameplayTagContainer` | ✅ | 許可アタッチメントタグ群 |
| `WeaponMesh1P` | `TSoftObjectPtr<USkeletalMesh>` | ✅ | 一人称用 Skeletal Mesh 参照 |
| `WeaponMesh3P` | `TSoftObjectPtr<USkeletalMesh>` |  | 三人称 / ドロップ用 Skeletal Mesh 参照 |
| `FireSound` | `TSoftObjectPtr<USoundBase>` | ✅ | 発砲音参照 |
| `MuzzleFlashFX` | `TSoftObjectPtr<UNiagaraSystem>` | ✅ | マズルフラッシュ FX 参照 |
| `ReloadMontage1P` | `TSoftObjectPtr<UAnimMontage>` |  | 一人称リロードモンタージュ |
| `EquipMontage1P` | `TSoftObjectPtr<UAnimMontage>` |  | 一人称装備切替モンタージュ |
| `AimOffsetCurve` | `TSoftObjectPtr<UCurveFloat>` |  | ADS 補間用カーブ |
| `IconTexture` | `TSoftObjectPtr<UTexture2D>` |  | UI アイコン |
| `Description` | `FText` |  | 図鑑 / 取引用説明文 |
| `SellCategoryTag` | `FGameplayTag` |  | 商人売却カテゴリタグ |
| `SpawnWeight` | `float` |  | Loot 抽選重み（0 なら通常抽選対象外） |
| `MinMerchantTier` | `int32` |  | 商人販売の最低 Tier |
| `IsStoryLocked` | `bool` | ✅ | ストーリー進行でロックされるか |
| `RequiredStoryFlag` | `FName` |  | `IsStoryLocked=true` 時の必要フラグ |

### CSV サンプル(該当する場合)

```csv
WeaponId,DisplayName,Category,Caliber,AmmoId,FireModes,BaseDamage,MuzzleVelocity,RPM,VerticalRecoil,HorizontalRecoil,Ergonomics,DefaultMagSize,ReloadTimeSec,TacticalReloadTimeSec,ADSTimeSec,WeightKg,DurabilityMax,BasePriceCredits,PriceVarianceRatio,AllowedAttachments,WeaponMesh1P,WeaponMesh3P,FireSound,MuzzleFlashFX,ReloadMontage1P,EquipMontage1P,AimOffsetCurve,IconTexture,Description,SellCategoryTag,SpawnWeight,MinMerchantTier,IsStoryLocked,RequiredStoryFlag
ar_556a_01,"AR-556A",AR,556,ammo_556_fmj,"Auto|Semi",44,910,780,0.95,0.62,58,30,2.80,2.20,0.23,3.40,100,32000,0.30,"Attachment.Muzzle.AR|Attachment.Sight.Rail|Attachment.Mag.AR556|Attachment.Grip.AR","/Game/BX/Weapons/AR556/Meshes/SK_BX_AR556_1P.SK_BX_AR556_1P","/Game/BX/Weapons/AR556/Meshes/SK_BX_AR556_3P.SK_BX_AR556_3P","/Game/BX/Audio/Weapons/SFX_BX_Gunshot_AR556.SFX_BX_Gunshot_AR556","/Game/BX/FX/Weapons/NS_BX_MuzzleFlash_AR.NS_BX_MuzzleFlash_AR","/Game/BX/Animations/Weapons/AR556/AM_BX_Reload_AR556_1P.AM_BX_Reload_AR556_1P","/Game/BX/Animations/Weapons/AR556/AM_BX_Equip_AR556_1P.AM_BX_Equip_AR556_1P","/Game/BX/Data/Common/Curves/CUR_BX_ADS_AR.CUR_BX_ADS_AR","/Game/BX/UI/Icons/T_BX_UI_Icon_AR556.T_BX_UI_Icon_AR556","Standard 5.56 assault rifle","Trade.Weapon.AR",0.65,2,false,
hg_9a_01,"HG-9A",HG,9mm,ammo_9mm_fmj,"Semi",34,365,420,0.48,0.21,76,15,1.85,1.45,0.14,0.90,100,8500,0.25,"Attachment.Muzzle.HG|Attachment.Sight.MiniRail|Attachment.Mag.HG9","/Game/BX/Weapons/HG9A/Meshes/SK_BX_HG9A_1P.SK_BX_HG9A_1P","/Game/BX/Weapons/HG9A/Meshes/SK_BX_HG9A_3P.SK_BX_HG9A_3P","/Game/BX/Audio/Weapons/SFX_BX_Gunshot_HG9.SFX_BX_Gunshot_HG9","/Game/BX/FX/Weapons/NS_BX_MuzzleFlash_HG.NS_BX_MuzzleFlash_HG","/Game/BX/Animations/Weapons/HG9A/AM_BX_Reload_HG9A_1P.AM_BX_Reload_HG9A_1P","/Game/BX/Animations/Weapons/HG9A/AM_BX_Equip_HG9A_1P.AM_BX_Equip_HG9A_1P","/Game/BX/Data/Common/Curves/CUR_BX_ADS_HG.CUR_BX_ADS_HG","/Game/BX/UI/Icons/T_BX_UI_Icon_HG9A.T_BX_UI_Icon_HG9A","Standard 9mm sidearm","Trade.Weapon.HG",0.95,1,false,
sr_308h_01,"SR-308H",SR,308,ammo_308_fmj,"Semi",92,835,50,1.75,0.72,38,5,3.30,2.70,0.34,5.40,100,62000,0.35,"Attachment.Muzzle.SR|Attachment.Sight.LongRail|Attachment.Mag.SR308","/Game/BX/Weapons/SR308H/Meshes/SK_BX_SR308H_1P.SK_BX_SR308H_1P","/Game/BX/Weapons/SR308H/Meshes/SK_BX_SR308H_3P.SK_BX_SR308H_3P","/Game/BX/Audio/Weapons/SFX_BX_Gunshot_SR308.SFX_BX_Gunshot_SR308","/Game/BX/FX/Weapons/NS_BX_MuzzleFlash_SR.NS_BX_MuzzleFlash_SR","/Game/BX/Animations/Weapons/SR308H/AM_BX_Reload_SR308H_1P.AM_BX_Reload_SR308H_1P","/Game/BX/Animations/Weapons/SR308H/AM_BX_Equip_SR308H_1P.AM_BX_Equip_SR308H_1P","/Game/BX/Data/Common/Curves/CUR_BX_ADS_SR.CUR_BX_ADS_SR","/Game/BX/UI/Icons/T_BX_UI_Icon_SR308H.T_BX_UI_Icon_SR308H","Long-range rifle","Trade.Weapon.SR",0.15,3,true,story_tokyo_midline_pass
```

### Blueprint / C++ 役割分担
- **C++ 側**:
  - `FBXWeaponTableRow` の定義
  - `ENUM_BX_WeaponCategory`, `ENUM_BX_FireMode` の定義
  - `AC_BX_WeaponRuntime` などランタイム参照コンポーネントから DataTable 行を読む処理
  - SoftObject の非同期ロードユーティリティ
- **Blueprint 側**:
  - `BP_` 派生武器 Actor への見た目・ソケット設定
  - DataTable 行に応じた Widget 表示
  - レベル配置と Loot コンテナの抽選設定
- **アセット配置先**:
  - C++: `Source/BX/Public/Data/Weapons/FBXWeaponTableRow.h`
  - C++: `Source/BX/Private/Data/Weapons/FBXWeaponTableRow.cpp`
  - DataTable: `Content/BX/Data/Common/Weapons/DT_BX_Weapons`
  - CSV ソース: `DataSource/Weapons/DT_BX_Weapons.csv`

### UI
武器詳細ポップアップは以下の情報を最低限表示する。

```text
┌──────────────────────────────────────┐
│ [武器名] AR-556A                    │
├──────────────────────────────────────┤
│ カテゴリ: AR    口径: 5.56          │
│ ダメージ: 44   RPM: 780             │
│ 縦反動: 0.95  横反動: 0.62          │
│ ADS: 0.23s    重量: 3.40kg          │
│ 耐久: 100/100  価格: 32000 ±30%     │
│ 対応: Muzzle / Sight / Mag / Grip   │
└──────────────────────────────────────┘
```

### 関連章
- §14-5
- §15-1
- §16-3

### 受け入れ条件
- [ ] `DT_BX_Weapons` に 10 件以上のサンプル行が登録されている
- [ ] `WeaponId` 重複時にインポートエラーまたは検証ログが出る
- [ ] `AllowedAttachments` を読み取って装着可否判定に使用できる
- [ ] `WeaponMesh1P`, `FireSound`, `MuzzleFlashFX` が SoftObject としてロード可能である
- [ ] `BasePriceCredits` と `PriceVarianceRatio` を使用して UI に価格レンジを表示できる

---

## §14-5 ダメージ計算モデル

### 概要
本章は、弾薬・防具・部位ダメージの交互作用を実装可能な数式として定義する。  
BX は「高難易度だが理不尽な即死だらけにはしない」方針のため、貫通判定・鈍的ダメージ・部位倍率・耐久劣化を明示的に分離する。

### 仕様
- 部位は **7部位**（Head / Chest / Abdomen / LeftArm / RightArm / LeftLeg / RightLeg）とする。
- 防具クラスは `Class1〜Class6` の 6 段階とする。
- 貫通判定は **弾の貫通値** と **防具の実効閾値** の比較で行う。
- 防具耐久が下がるほど、実効閾値は低下する。
- 非貫通時でも鈍的ダメージは発生する。
- 貫通時の本体ダメージは減衰係数を通して部位へ適用する。
- 腕・脚は戦闘不能リスクではなく操作劣化を重視する。
- 頭部は通常は致命的だが、`Neuro Critical System`（別章）を別条件で上書き可能とする。
- 乱数はサーバ不要のオフライン設計だが、同一条件で極端なブレを避けるため乱数範囲は狭くする。
- ショットガン散弾は **1ペレット単位** で貫通判定を行う。

### データ構造

#### §14-5-1 防具クラス基準表

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `ArmorClass` | `int32` | ✅ | 防具クラス（1〜6） |
| `BasePenetrationThreshold` | `float` | ✅ | 耐久100%時の基準貫通閾値 |
| `BluntThroughputRatio` | `float` | ✅ | 非貫通時の鈍的ダメージ比率 |
| `DurabilityDamageMultiplier` | `float` | ✅ | 被弾時の耐久減少補正 |
| `RepairLossRatio` | `float` | ✅ | 修理時の最大耐久低下率 |

| ArmorClass | BasePenetrationThreshold | BluntThroughputRatio | DurabilityDamageMultiplier | RepairLossRatio |
|---|---:|---:|---:|---:|
| 1 | 10.0 | 0.28 | 1.20 | 0.08 |
| 2 | 18.0 | 0.24 | 1.10 | 0.10 |
| 3 | 28.0 | 0.20 | 1.00 | 0.12 |
| 4 | 38.0 | 0.16 | 0.90 | 0.15 |
| 5 | 48.0 | 0.13 | 0.82 | 0.18 |
| 6 | 58.0 | 0.10 | 0.75 | 0.22 |

#### §14-5-2 代表弾データ表

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `AmmoId` | `FName` | ✅ | 主キー |
| `DisplayName` | `FText` | ✅ | 弾薬名 |
| `Caliber` | `FName` | ✅ | 口径 |
| `Penetration` | `float` | ✅ | 貫通値 |
| `Damage` | `float` | ✅ | 基本ダメージ |
| `ArmorDamageRatio` | `float` | ✅ | 防具耐久削り補正 |
| `FragmentChance` | `float` | ✅ | 破片化確率（0.00〜1.00） |
| `VelocityFactor` | `float` | ✅ | 初速補正係数 |

| AmmoId | DisplayName | Caliber | Penetration | Damage | ArmorDamageRatio | FragmentChance | VelocityFactor |
|---|---|---|---:|---:|---:|---:|---:|
| `ammo_9mm_fmj` | 9mm FMJ | `9mm` | 12.0 | 34.0 | 0.90 | 0.03 | 1.00 |
| `ammo_9mm_jhp` | 9mm JHP | `9mm` | 8.0 | 40.0 | 0.75 | 0.08 | 0.98 |
| `ammo_57_ap` | 5.7 AP | `57` | 24.0 | 29.0 | 1.00 | 0.02 | 1.05 |
| `ammo_556_fmj` | 5.56 FMJ | `556` | 32.0 | 44.0 | 1.05 | 0.05 | 1.00 |
| `ammo_556_ap` | 5.56 AP | `556` | 39.0 | 41.0 | 1.10 | 0.03 | 1.02 |
| `ammo_762_fmj` | 7.62 FMJ | `762` | 38.0 | 58.0 | 1.12 | 0.06 | 0.97 |
| `ammo_762_ap` | 7.62 AP | `762` | 47.0 | 54.0 | 1.18 | 0.03 | 0.99 |
| `ammo_12g_pellet` | 12G Pellet | `12G` | 3.0 | 11.0 | 0.55 | 0.00 | 0.90 |
| `ammo_12g_slug` | 12G Slug | `12G` | 18.0 | 88.0 | 0.95 | 0.02 | 0.93 |
| `ammo_308_fmj` | .308 FMJ | `308` | 46.0 | 72.0 | 1.15 | 0.05 | 1.00 |
| `ammo_308_ap` | .308 AP | `308` | 56.0 | 68.0 | 1.22 | 0.02 | 1.01 |

#### §14-5-3 部位ダメージ倍率表

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `BodyPart` | `ENUM_BX_BodyPart` | ✅ | 命中部位 |
| `DamageMultiplier` | `float` | ✅ | 最終ダメージ倍率 |
| `BleedSmallBonus` | `float` | ✅ | 小出血補正係数 |
| `BleedLargeBonus` | `float` | ✅ | 大出血補正係数 |
| `FractureChanceBonus` | `float` | ✅ | 骨折補正係数 |

| BodyPart | DamageMultiplier | BleedSmallBonus | BleedLargeBonus | FractureChanceBonus |
|---|---:|---:|---:|---:|
| `Head` | 4.00 | 0.20 | 0.65 | 0.00 |
| `Chest` | 1.00 | 0.18 | 0.30 | 0.00 |
| `Abdomen` | 1.25 | 0.28 | 0.40 | 0.00 |
| `LeftArm` | 0.72 | 0.15 | 0.18 | 0.35 |
| `RightArm` | 0.72 | 0.15 | 0.18 | 0.35 |
| `LeftLeg` | 0.82 | 0.12 | 0.15 | 0.45 |
| `RightLeg` | 0.82 | 0.12 | 0.15 | 0.45 |

### 仕様（計算式）

#### §14-5-4 実効防具閾値
```text
ArmorDurabilityRatio = clamp(CurrentDurability / MaxDurability, 0.0, 1.0)

EffectivePenetrationThreshold
  = BasePenetrationThreshold * lerp(0.55, 1.00, ArmorDurabilityRatio)
```

#### §14-5-5 貫通スコア
```text
PenetrationScore = AmmoPenetration - EffectivePenetrationThreshold
```

#### §14-5-6 貫通確率
```text
if PenetrationScore <= -15.0:
    PenetrationChance = 0.0
else if PenetrationScore >= 15.0:
    PenetrationChance = 1.0
else:
    PenetrationChance = 0.5 + (PenetrationScore / 30.0)
```

#### §14-5-7 貫通成功時ダメージ
```text
PenetrationDamageFalloff
  = clamp(1.0 - max(0.0, EffectivePenetrationThreshold - AmmoPenetration) * 0.01, 0.70, 1.00)

FinalPenetratedDamage
  = AmmoDamage * BodyPartDamageMultiplier * PenetrationDamageFalloff
```

#### §14-5-8 非貫通時の鈍的ダメージ
```text
FinalBluntDamage
  = AmmoDamage * BodyPartDamageMultiplier * BluntThroughputRatio
```

#### §14-5-9 防具耐久減少
```text
ArmorDurabilityDamage
  = AmmoDamage * ArmorDamageRatio * DurabilityDamageMultiplier
```

#### §14-5-10 散弾
- 12G 散弾は **1ペレットごと** に `§14-5-4`〜`§14-5-9` を適用する。
- 同一部位に複数ペレット命中した場合は合算する。
- 同一フレーム大量ヒット時は UI ダメージ表示を1件に統合してよい。

### CSV サンプル(該当する場合)

```csv
ArmorClass,BasePenetrationThreshold,BluntThroughputRatio,DurabilityDamageMultiplier,RepairLossRatio
1,10.0,0.28,1.20,0.08
2,18.0,0.24,1.10,0.10
3,28.0,0.20,1.00,0.12
4,38.0,0.16,0.90,0.15
5,48.0,0.13,0.82,0.18
6,58.0,0.10,0.75,0.22
```

```csv
AmmoId,DisplayName,Caliber,Penetration,Damage,ArmorDamageRatio,FragmentChance,VelocityFactor
ammo_9mm_fmj,"9mm FMJ",9mm,12,34,0.90,0.03,1.00
ammo_556_fmj,"5.56 FMJ",556,32,44,1.05,0.05,1.00
ammo_762_ap,"7.62 AP",762,47,54,1.18,0.03,0.99
ammo_308_ap,".308 AP",308,56,68,1.22,0.02,1.01
```

### Blueprint / C++ 役割分担
- **C++ 側**:
  - `UBXDamageCalculatorSubsystem` または同等 Subsystem で計算式を実装
  - `ENUM_BX_BodyPart` の定義
  - 防具耐久減少・部位ダメージ・出血 / 骨折イベント通知
  - 散弾多段ヒットの集計処理
- **Blueprint 側**:
  - 部位別ヒットリアクション
  - UI ダメージ表示
  - ヒットエフェクト切替
  - `Neuro Critical` 演出トリガー
- **アセット配置先**:
  - C++: `Source/BX/Public/Systems/Combat/BXDamageCalculatorSubsystem.h`
  - C++: `Source/BX/Private/Systems/Combat/BXDamageCalculatorSubsystem.cpp`
  - 共通データ: `Content/BX/Data/Common/Combat/`
  - CSV ソース: `DataSource/Combat/DT_BX_ArmorClasses.csv`, `DataSource/Combat/DT_BX_AmmoBallistics.csv`

### UI
被弾デバッグ表示（開発用）は以下を推奨する。

```text
┌──────────────────────────────────────┐
│ Hit: Chest                           │
│ Ammo: ammo_556_fmj                   │
│ PenScore: -3.2   PenChance: 0.39     │
│ Result: NonPen / Blunt 7.0           │
│ ArmorDurabilityDamage: 41.6          │
└──────────────────────────────────────┘
```

### 関連章
- §14-4
- §15-2
- §16-3

### 受け入れ条件
- [ ] 防具クラス 1〜6 の基準表が DataTable または定数テーブルで参照可能である
- [ ] 代表弾 10 種以上で `Penetration`, `Damage`, `ArmorDamageRatio` が設定されている
- [ ] 防具耐久100% と 20% で同一弾の貫通率差を確認できる
- [ ] 非貫通時にも `FinalBluntDamage` が適用される
- [ ] 7部位それぞれで倍率差がログまたは HUD デバッグ表示で確認できる

---

## §15-1 アイテム DataTable とインベントリ基本構造

### 概要
`DT_BX_Items` は BX の全アイテム共通マスタであり、インベントリ・Loot・商人取引・依頼納品の基準データを提供する。  
本章はアイテム共通行構造、インベントリスロット、スタック条件、重量計算を定義する。

### 仕様
- DataTable 名は `DT_BX_Items` とする。
- 行構造体名は `FBXItemTableRow` とする。
- 武器本体は `DT_BX_Weapons` で管理するが、インベントリ上の表示 / 売買 / 依頼条件では `DT_BX_Items` と相互参照できるようにする。
- アイテムカテゴリは `ENUM_BX_ItemCategory` で管理する。
- 1 スタック上限は `MaxStackCount` で定義する。
- 重量はアイテム 1 個あたり `UnitWeightKg` とし、総重量は `Quantity * UnitWeightKg` で計算する。
- 耐久を持たないアイテムは `bUsesDurability=false` とする。
- マガジン、消耗品、紙幣、依頼品、鍵、医療品は武器とは別カテゴリで扱う。
- `BasePriceCredits` と `PriceVarianceRatio` により売買の代表値を定義する。
- ストーリーロック品と依頼専用品は `bIsTradeable=false` にできる。
- 日本円コレクター対象品は `CollectorSeriesId` を設定し、弾薬通貨変換率は別テーブルで上書き可能とする。

### データ構造

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `ItemId` | `FName` | ✅ | 主キー。例: `med_bandage_01` |
| `DisplayName` | `FText` | ✅ | 表示名 |
| `Category` | `ENUM_BX_ItemCategory` | ✅ | アイテムカテゴリ |
| `SubCategoryTag` | `FGameplayTag` |  | 細分類タグ |
| `BasePriceCredits` | `int32` | ✅ | 基本価格（弾価値クレジット） |
| `PriceVarianceRatio` | `float` | ✅ | 価格変動率 |
| `UnitWeightKg` | `float` | ✅ | 1 個あたり重量（kg） |
| `MaxStackCount` | `int32` | ✅ | 1 スタック上限 |
| `bStackable` | `bool` | ✅ | スタック可能か |
| `bUsesDurability` | `bool` | ✅ | 耐久を持つか |
| `DurabilityMax` | `float` |  | 最大耐久値 |
| `bIsQuestItem` | `bool` | ✅ | 依頼品か |
| `bIsTradeable` | `bool` | ✅ | 商人売買可能か |
| `bCanStoreAtMerchant` | `bool` | ✅ | 商人預け可能か |
| `bCanBringIntoRaid` | `bool` | ✅ | レイド持込可能か |
| `InventorySlotType` | `ENUM_BX_InventorySlotType` | ✅ | 主な収納先 |
| `IconTexture` | `TSoftObjectPtr<UTexture2D>` |  | アイコン |
| `WorldMesh` | `TSoftObjectPtr<UStaticMesh>` |  | フィールドドロップメッシュ |
| `Description` | `FText` |  | 説明文 |
| `UseActionId` | `FName` |  | 使用アクション ID |
| `CollectorSeriesId` | `FName` |  | コレクター収集系 ID |
| `StoryFlagRequired` | `FName` |  | 取得 / 表示に必要なストーリーフラグ |

#### §15-1-1 インベントリスロット型

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `SlotId` | `FName` | ✅ | スロット識別子 |
| `SlotType` | `ENUM_BX_InventorySlotType` | ✅ | スロット種別 |
| `CapacityUnits` | `int32` | ✅ | 収納容量単位 |
| `bAcceptsWeapon` | `bool` | ✅ | 武器収納可否 |
| `bAcceptsMagazine` | `bool` | ✅ | マガジン収納可否 |
| `bAcceptsConsumable` | `bool` | ✅ | 消耗品収納可否 |
| `bAcceptsQuestItem` | `bool` | ✅ | 依頼品収納可否 |

#### §15-1-2 ランタイムスタック構造

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `ItemId` | `FName` | ✅ | アイテム ID |
| `Quantity` | `int32` | ✅ | 所持数 |
| `CurrentDurability` | `float` |  | 現在耐久 |
| `AmmoLoadedCount` | `int32` |  | マガジン装弾数 |
| `CustomTag` | `FName` |  | 特殊状態タグ |
| `SourceRegionId` | `FName` |  | 取得地域 ID |
| `bIsFoundInRaid` | `bool` | ✅ | レイド内取得品か |

### CSV サンプル(該当する場合)

```csv
ItemId,DisplayName,Category,SubCategoryTag,BasePriceCredits,PriceVarianceRatio,UnitWeightKg,MaxStackCount,bStackable,bUsesDurability,DurabilityMax,bIsQuestItem,bIsTradeable,bCanStoreAtMerchant,bCanBringIntoRaid,InventorySlotType,IconTexture,WorldMesh,Description,UseActionId,CollectorSeriesId,StoryFlagRequired
med_bandage_01,"Bandage",Medical,Item.Medical.Bandage,300,0.15,0.10,2,true,false,0,false,true,true,true,QuickUse,"/Game/BX/UI/Icons/T_BX_UI_Icon_Bandage.T_BX_UI_Icon_Bandage","/Game/BX/Items/Medical/Meshes/SM_BX_Bandage.SM_BX_Bandage","Stops small bleed",use_bandage,,
food_ration_01,"Ration Pack",Food,Item.Food.Ration,500,0.20,0.35,3,true,false,0,false,true,true,true,Backpack,"/Game/BX/UI/Icons/T_BX_UI_Icon_Ration.T_BX_UI_Icon_Ration","/Game/BX/Items/Food/Meshes/SM_BX_Ration.SM_BX_Ration","Restores energy",use_food,,
cash_note_2020_01,"Old Yen Note 2020",Collectible,Item.Collectible.CashNote,1200,0.60,0.01,50,true,false,0,false,true,true,true,Pouch,"/Game/BX/UI/Icons/T_BX_UI_Icon_YenNote.T_BX_UI_Icon_YenNote","/Game/BX/Items/Collectibles/Meshes/SM_BX_YenNote.SM_BX_YenNote","Collector item",,collector_yen_notes,
key_lab_roof_01,"Roof Access Key",Key,Item.Key.Story,0,0.00,0.02,1,false,false,0,false,false,true,true,Keyring,"/Game/BX/UI/Icons/T_BX_UI_Icon_Key.T_BX_UI_Icon_Key","/Game/BX/Items/Keys/Meshes/SM_BX_Key.SM_BX_Key","Unlocks roof extract",,,"story_research_roof_access"
```

### Blueprint / C++ 役割分担
- **C++ 側**:
  - `FBXItemTableRow`, `FBXInventorySlotDef`, `FBXInventoryItemStack` 定義
  - `AC_BX_Inventory` の容量計算、スタック統合、重量再計算
  - スロット可否判定と保存用シリアライズ
- **Blueprint 側**:
  - `WBP_BX_InventoryGrid` による一覧表示
  - ドラッグ＆ドロップ UI
  - レベル上の Loot コンテナの見た目とソート順
- **アセット配置先**:
  - C++: `Source/BX/Public/Data/Items/FBXItemTableRow.h`
  - C++: `Source/BX/Public/Items/Components/AC_BX_Inventory.h`
  - DataTable: `Content/BX/Data/Common/Items/DT_BX_Items`
  - UI: `Content/BX/UI/Inventory/WBP_BX_InventoryGrid`
  - CSV ソース: `DataSource/Items/DT_BX_Items.csv`

### UI

```text
┌────────────────────────────────────────────┐
│ 装備重量: 18.4kg / 許容 24.0kg            │
├───────────────┬────────────────────────────┤
│ 装備スロット       │ バックパック / ポーチ      │
│ Main Weapon        │ [Bandage x2]           │
│ Sidearm            │ [Ration x1]            │
│ Armor              │ [Old Yen Note x12]     │
│ QuickUse 1..4      │ [Roof Access Key]      │
└───────────────┴────────────────────────────┘
```

### 関連章
- §14-4
- §16-3
- §29-3

### 受け入れ条件
- [ ] `DT_BX_Items` に 20 件以上のサンプルデータが存在する
- [ ] `AC_BX_Inventory` がスタック統合と重量再計算を行う
- [ ] `bIsTradeable=false` のアイテムは商人売却リストに表示されない
- [ ] 紙幣系アイテムに `CollectorSeriesId` を設定できる
- [ ] レイド中取得品に `bIsFoundInRaid=true` を付与できる

---

## §15-2 状態異常 DataTable と Neuro Critical 基本定義

### 概要
BX の高難易度感は、単純な HP 減少ではなく、出血・骨折・疼痛・脱水・疲労・Neuro Critical の複合によって成立する。  
本章は `DT_BX_StatusEffects` の行構造と、各状態異常の共通パラメータを定義する。

### 仕様
- DataTable 名は `DT_BX_StatusEffects` とする。
- 行構造体名は `FBXStatusEffectTableRow` とする。
- 状態異常は `ENUM_BX_StatusType` で分類する。
- Tick 更新は `TickIntervalSec` 基準で管理する。
- 時間制限がない永続異常は `bUsesDuration=false` とする。
- `Neuro Critical` は通常異常とは別格だが、同一テーブル管理とし、処置可能アイテムは `CureActionId` で紐付ける。
- 同一異常の重複可否は `bCanStack` で制御する。
- UI 優先度は `UISeverityLevel` で制御する。
- 足・腕の骨折は部位別タグで区別し、基礎定義は共通行を使う。
- 大出血は小出血を上書きしてよい。

### データ構造

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `StatusId` | `FName` | ✅ | 主キー |
| `DisplayName` | `FText` | ✅ | 表示名 |
| `StatusType` | `ENUM_BX_StatusType` | ✅ | 状態異常分類 |
| `TickIntervalSec` | `float` | ✅ | 更新間隔（秒） |
| `bUsesDuration` | `bool` | ✅ | 時間制限を持つか |
| `BaseDurationSec` | `float` |  | 基本継続時間（秒） |
| `DamagePerTick` | `float` |  | Tick ごとのダメージ |
| `MoveSpeedRatio` | `float` | ✅ | 移動速度倍率 |
| `ADSSpeedRatio` | `float` | ✅ | ADS 速度倍率 |
| `StaminaRecoveryRatio` | `float` | ✅ | スタミナ回復倍率 |
| `AimStabilityRatio` | `float` | ✅ | 照準安定倍率 |
| `AudioMuffleRatio` | `float` | ✅ | 聴覚低下倍率 |
| `bCanStack` | `bool` | ✅ | 重複可能か |
| `MaxStackCount` | `int32` | ✅ | 最大スタック数 |
| `UISeverityLevel` | `int32` | ✅ | UI 表示優先度 1〜5 |
| `CureActionId` | `FName` |  | 治療アクション ID |
| `RequiredMedItemId` | `FName` |  | 必要医療品 ID |
| `Description` | `FText` |  | 説明文 |

### CSV サンプル(該当する場合)

```csv
StatusId,DisplayName,StatusType,TickIntervalSec,bUsesDuration,BaseDurationSec,DamagePerTick,MoveSpeedRatio,ADSSpeedRatio,StaminaRecoveryRatio,AimStabilityRatio,AudioMuffleRatio,bCanStack,MaxStackCount,UISeverityLevel,CureActionId,RequiredMedItemId,Description
status_small_bleed,"Small Bleed",Bleed,1.0,false,0,1.2,1.00,1.00,0.90,0.98,1.00,false,1,3,cure_small_bleed,med_bandage_01,"Light bleeding over time"
status_large_bleed,"Large Bleed",Bleed,1.0,false,0,4.5,0.96,0.95,0.65,0.90,1.00,false,1,5,cure_large_bleed,med_tourniquet_01,"Severe blood loss"
status_leg_fracture,"Leg Fracture",Fracture,0.5,false,0,0,0.65,1.00,0.85,0.95,1.00,false,1,4,cure_fracture,med_splint_01,"Heavy movement penalty"
status_neuro_critical,"Neuro Critical",Neuro,0.2,true,15,0,0.60,0.55,0.50,0.50,0.70,false,1,5,cure_neuro_critical,med_n15_01,"Temporary survival window after lethal head trauma"
```

### Blueprint / C++ 役割分担
- **C++ 側**:
  - `FBXStatusEffectTableRow` 定義
  - `AC_BX_StatusEffects` コンポーネント実装
  - Tick 処理、重複制御、治療判定
  - `Neuro Critical` の残時間と一回制限管理
- **Blueprint 側**:
  - 状態異常アイコン表示
  - 画面エフェクト / 音響劣化演出
  - 重傷時の HUD 点滅
- **アセット配置先**:
  - C++: `Source/BX/Public/Data/Status/FBXStatusEffectTableRow.h`
  - C++: `Source/BX/Public/Characters/Components/AC_BX_StatusEffects.h`
  - DataTable: `Content/BX/Data/Common/Status/DT_BX_StatusEffects`
  - UI: `Content/BX/UI/HUD/Status/`
  - CSV ソース: `DataSource/Status/DT_BX_StatusEffects.csv`

### UI

```text
┌──────────────────────────────────────┐
│ [BLEED] [PAIN] [NEURO]               │
│ Neuro Critical: 12.8s                │
│ Use N-15 immediately                 │
└──────────────────────────────────────┘
```

### 関連章
- §14-5
- §15-1
- §16-3

### 受け入れ条件
- [ ] `DT_BX_StatusEffects` に 8 件以上の状態異常サンプルがある
- [ ] 大出血が小出血を上書きする
- [ ] 骨折が移動速度へ反映される
- [ ] `Neuro Critical` が 15 秒の残時間を持つ
- [ ] `RequiredMedItemId` を満たす治療アイテムでのみ解除できる

---

## §29-3 商人ハブのホーム画面 UI

### 概要
`WBP_BX_MerchantHub` は、商人ネットワーク方式をプレイヤーが理解し、取引・依頼・再開地点・在庫預けをひとつの場所で回すための中心 UI である。  
本章はホーム画面の ASCII レイアウト、表示項目、入力ルール、データ接続先を定義する。

### 仕様
- Widget 名は `WBP_BX_MerchantHub` とする。
- ハブは「ホーム」「取引」「在庫預け」「依頼」「再開地点」「情報」の 6 タブ構成とする。
- ホーム画面は各タブの入口と、現在状態の要約を担う。
- 左カラムに商人一覧、中央に現在商人の要約、右カラムにプレイヤー状態と最近変動を表示する。
- デフォルト選択は「最後に訪れた商人」があればその商人、なければミナトとする。
- Trust 表示は「Level + 現在ポイント / 次レベル必要値」で示す。
- 預け在庫数、解放済み再開地点数、進行中依頼数、特記事項（警報・不足物資）をホーム画面で見えるようにする。
- 取引価格は §15-1 の `BasePriceCredits` と `PriceVarianceRatio`、および商人補正で算出した現在値を表示する。
- 商人固有の短文見出しを表示する。
- `ESC` / `B` でハブを閉じる。未保存の操作がある場合は確認ダイアログを出す。
- ゲームパッド / キーボード両対応前提で、タブ切替は上段、商人切替は左リストとする。

### データ構造

#### §29-3-1 ホーム画面 ViewModel

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `MerchantId` | `FName` | ✅ | 現在選択商人 ID |
| `MerchantDisplayName` | `FText` | ✅ | 商人名 |
| `MerchantTagline` | `FText` | ✅ | 商人見出し文 |
| `TrustLevel` | `int32` | ✅ | 現在 Trust レベル |
| `TrustPoints` | `int32` | ✅ | 現在 Trust ポイント |
| `TrustPointsToNext` | `int32` | ✅ | 次段階まで残りポイント |
| `StoredInventoryCount` | `int32` | ✅ | 預け在庫総数 |
| `UnlockedRespawnNodeCount` | `int32` | ✅ | 解放済み再開地点数 |
| `ActiveQuestCount` | `int32` | ✅ | 進行中依頼数 |
| `PlayerCredits` | `int32` | ✅ | プレイヤー所持クレジット |
| `CurrentRegionId` | `FName` | ✅ | 現在地域 ID |
| `RecentLogLines` | `TArray<FText>` | ✅ | 最近の変動ログ |
| `WarningTags` | `FGameplayTagContainer` |  | 不足物資 / 重傷 / 期限警告など |

#### §29-3-2 商人一覧行 ViewModel

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `MerchantId` | `FName` | ✅ | 商人 ID |
| `DisplayName` | `FText` | ✅ | 表示名 |
| `TrustLevel` | `int32` | ✅ | Trust レベル |
| `bHasNewQuest` | `bool` | ✅ | 新規依頼あり |
| `bHasUrgentWarning` | `bool` | ✅ | 緊急注意あり |
| `IconTexture` | `TSoftObjectPtr<UTexture2D>` |  | 顔 / アイコン |

### Blueprint / C++ 役割分担
- **C++ 側**:
  - `UBXMerchantHubSubsystem` で商人状態集約
  - `FBXMerchantHubHomeViewModel` 相当構造体定義
  - SaveGame / DataTable から UI へ渡す整形処理
- **Blueprint 側**:
  - `WBP_BX_MerchantHub` の見た目実装
  - タブ切替、アニメーション、リスト選択
  - 色・アイコン・文言反映
- **アセット配置先**:
  - C++: `Source/BX/Public/UI/MerchantHub/BXMerchantHubSubsystem.h`
  - C++: `Source/BX/Public/UI/MerchantHub/FBXMerchantHubViewModels.h`
  - Widget: `Content/BX/UI/MerchantHub/WBP_BX_MerchantHub`
  - 子 Widget: `Content/BX/UI/MerchantHub/Components/`
  - アイコン: `Content/BX/UI/MerchantHub/Icons/`

### UI

```text
┌─────────────────────────────────────────────────────────────────────┐
│ 商人ハブ  [Home] [Trade] [Storage] [Quests] [Respawn] [Intel]      │
├──────────────────┬────────────────────────────────┬──────────────────┤
│ 商人一覧           │ 現在商人: 古物商 ミナト         │ プレイヤー状態       │
│ > ミナト Lv2      │ 「持ち帰った物にだけ価値がある」 │ Credits: 1840        │
│   サワベ Lv1      │ Trust: 145 / 260              │ Region: Kansai...    │
│   クロガネ Lv2    │ 預け在庫: 28                  │ Active Quests: 3     │
│   ヨシナガ Lv1    │ 再開地点: 2                   │ Warnings: BleedRisk  │
│   アマギ Lv1      │ 新規依頼: 1                   │                      │
├──────────────────┼────────────────────────────────┼──────────────────┤
│                  │ 最近の変動                        │ 最近のログ          │
│                  │ ・港区搬出品の売却率上昇          │ + Trust Minato      │
│                  │ ・港周辺再開地点候補あり          │ + Quest Progress    │
│                  │ ・取引手数料: 標準                │ - Ammo Credits      │
└──────────────────┴────────────────────────────────┴──────────────────┘
```

### 関連章
- §15-1
- §16-3
- §29-4

### 受け入れ条件
- [ ] `WBP_BX_MerchantHub` に 6 タブが存在する
- [ ] 左側商人一覧で選択した商人の情報が中央へ反映される
- [ ] Trust レベルと次段階までの残り値が表示される
- [ ] 預け在庫数、解放再開地点数、進行中依頼数がホーム画面に表示される
- [ ] `RecentLogLines` が 3 行以上表示される


---

## §18-2 抽出条件データ構造

### 概要
`DT_BX_ExtractRules` は、BX の抽出地点ごとの解放条件、失敗条件、待機時間、警報条件、重量条件、増援発生を管理する抽出ルール DataTable である。  
近い抽出ほど危険・条件付き、遠い抽出ほど安定という BX の設計思想を、Claude Code が C++ と CSV に落とし込めるよう行構造で定義する。

### 仕様
- DataTable 名は `DT_BX_ExtractRules` とする。
- 行構造体名は `FBXExtractRuleRow` とする。
- 主キーは `ExtractId` とし、同一 `RegionId` 内で一意とする。
- 抽出は **候補表示** と **実際の使用可否** を分けて扱う。
- 条件は AND 条件で判定し、UI には未達成条件一覧を表示する。
- `CountdownTimeSec` は抽出待機時間であり、0 の場合は即時抽出扱いとする。
- 待機中断条件は `bCancelOnDamage`, `bCancelOnMoveOut`, `bCancelOnAim`, `bCancelOnUseItem` の個別フラグで制御する。
- 商人依存抽出は `RequiredMerchantId` と `RequiredTrustLevel` を併用する。
- 重量条件は `RequiredMaxWeightKg` で表す。0 以下の場合は制限なしとする。
- 警報条件は「この値以下であること」を基準にする。
- 電源、鍵、特定消耗品、特定依頼フラグ、時刻帯を要求できる。
- 抽出発動時に増援を出す場合、`ReinforcementWaveId` を参照する。
- マップ上の見え方は `MarkerVisibilityType` で制御する。
- 物理的な抽出トリガー Actor は `BP_BX_ExtractPointBase` を C++ 基底 + Blueprint 派生で実装する。

### データ構造

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `ExtractId` | `FName` | ✅ | 主キー。例: `extract_port_checkpoint_east` |
| `DisplayName` | `FText` | ✅ | 抽出地点表示名 |
| `RegionId` | `FName` | ✅ | 所属地域 ID |
| `SubLevelId` | `FName` |  | 区画 / サブレベル ID |
| `ExtractType` | `ENUM_BX_ExtractType` | ✅ | 抽出種別（Checkpoint / Rope / Vehicle / Hidden / Merchant 等） |
| `TriggerActorTag` | `FName` | ✅ | ワールド上の抽出トリガー Actor タグ |
| `bVisibleOnMapByDefault` | `bool` | ✅ | 初期表示されるか |
| `MarkerVisibilityType` | `ENUM_BX_ExtractMarkerVisibility` | ✅ | マーカー表示種別 |
| `RequiredKeyItemId` | `FName` |  | 必要鍵アイテム ID |
| `RequiredConsumableItemId` | `FName` |  | 消費型必要アイテム ID |
| `RequiredMerchantId` | `FName` |  | 必要商人 ID |
| `RequiredTrustLevel` | `int32` |  | 必要 Trust レベル |
| `RequiredQuestId` | `FName` |  | 必要依頼 ID |
| `RequiredStoryFlag` | `FName` |  | 必要ストーリーフラグ |
| `RequiredPowerTag` | `FName` |  | 必要通電 / 制御タグ |
| `MaxAllowedAlertLevel` | `int32` | ✅ | 使用可能な最大警報レベル |
| `RequiredMaxWeightKg` | `float` | ✅ | 使用可能な最大重量（kg） |
| `RequiredTimeWindowTag` | `FName` |  | 必要時刻帯タグ。例: `Time.Night` |
| `CountdownTimeSec` | `float` | ✅ | 抽出待機時間（秒） |
| `bCancelOnDamage` | `bool` | ✅ | 被弾で中断するか |
| `bCancelOnMoveOut` | `bool` | ✅ | 範囲外移動で中断するか |
| `bCancelOnAim` | `bool` | ✅ | ADS で中断するか |
| `bCancelOnUseItem` | `bool` | ✅ | アイテム使用で中断するか |
| `bConsumesRequiredItem` | `bool` | ✅ | 必要アイテムを消費するか |
| `ReinforcementWaveId` | `FName` |  | 抽出発動時の増援 Wave ID |
| `UIHintText` | `FText` |  | UI 補助文 |
| `PriorityOrder` | `int32` | ✅ | UI 並び順 |
| `bOneWayOnly` | `bool` | ✅ | 一方向専用抽出か |
| `bDisabledInExReal` | `bool` | ✅ | EXリアルで無効か |

### CSV サンプル(該当する場合)

```csv
ExtractId,DisplayName,RegionId,SubLevelId,ExtractType,TriggerActorTag,bVisibleOnMapByDefault,MarkerVisibilityType,RequiredKeyItemId,RequiredConsumableItemId,RequiredMerchantId,RequiredTrustLevel,RequiredQuestId,RequiredStoryFlag,RequiredPowerTag,MaxAllowedAlertLevel,RequiredMaxWeightKg,RequiredTimeWindowTag,CountdownTimeSec,bCancelOnDamage,bCancelOnMoveOut,bCancelOnAim,bCancelOnUseItem,bConsumesRequiredItem,ReinforcementWaveId,UIHintText,PriorityOrder,bOneWayOnly,bDisabledInExReal
extract_east_checkpoint,"East Checkpoint",kansai_industrial_01,checkpoint_east,Checkpoint,extract_checkpoint_east,true,AlwaysVisible,,,,,,power_none,3,0,,12.0,true,true,false,false,false,wave_extract_checkpoint_east,"Hold position until gate cycle completes",10,false,false
extract_port_hidden,"Port Hidden Route",kansai_industrial_01,port_south,Hidden,extract_port_hidden,false,DiscoveredOnly,key_port_hidden_01,,merchant_minato,2,quest_minato_route_02,,power_none,1,18.0,Time.Night,5.0,true,true,true,true,false,wave_extract_port_hidden,"Requires Minato access and low profile movement",20,true,true
extract_roof_cable,"Roof Cable Exit",research_center_01,roof_access,Rope,extract_roof_cable,false,Conditional,key_lab_roof_01,consumable_cable_hook_01,,,quest_research_roof_01,story_research_roof_access,power_roof_online,2,22.0,,8.0,true,true,false,true,true,wave_extract_roof_cable,"Roof power and access key required",30,true,false
extract_courier_pickup,"Courier Pickup",kansai_industrial_01,under_highway,Merchant,extract_courier_pickup,true,AlwaysVisible,,item_pickup_token_01,merchant_yoshinaga,2,,story_courier_contact,signal_pickup_online,2,30.0,,20.0,true,true,false,false,true,wave_extract_courier_pickup,"Wait for courier van arrival",40,false,false
```

### Blueprint / C++ 役割分担

- **C++ 側**:
  - `FBXExtractRuleRow` 定義
  - `AC_BX_ExtractEvaluator` で使用可否判定
  - `ABXExtractPointBase`（または同等基底）で待機タイマー、条件確認、成功通知
  - SaveGame との抽出解放状態同期
- **Blueprint 側**:
  - `BP_BX_ExtractPointBase` 派生ごとの見た目
  - 抽出地点ごとの演出、音、ライト、ロープ挙動
  - UI への未達条件表示
- **アセット配置先**:
  - C++: `Source/BX/Public/Data/Extract/FBXExtractRuleRow.h`
  - C++: `Source/BX/Public/World/Extract/AC_BX_ExtractEvaluator.h`
  - C++: `Source/BX/Public/World/Extract/ABXExtractPointBase.h`
  - DataTable: `Content/BX/Data/Common/Extract/DT_BX_ExtractRules`
  - Blueprint: `Content/BX/World/Extract/`
  - CSV ソース: `DataSource/Extract/DT_BX_ExtractRules.csv`

### UI

```text
┌──────────────────────────────────────────────┐
│ 抽出地点: Roof Cable Exit                   │
├──────────────────────────────────────────────┤
│ 状態: 使用不可                               │
│ 未達条件:                                    │
│ ・Roof Access Key が必要                     │
│ ・Cable Hook が必要                          │
│ ・警報レベル 2 以下が必要                    │
│ ・重量 22.0kg 以下が必要                     │
├──────────────────────────────────────────────┤
│ 待機時間: 8.0秒                              │
│ 中断条件: 被弾 / 範囲外 / アイテム使用       │
└──────────────────────────────────────────────┘
```

### 関連章
- §15-1
- §16-3
- §29-3

### 受け入れ条件
- [ ] `DT_BX_ExtractRules` に 6 件以上のサンプル抽出が登録されている
- [ ] 鍵、商人 Trust、依頼、警報、重量の条件を個別に ON/OFF できる
- [ ] 未達条件一覧が UI に表示される
- [ ] `CountdownTimeSec` が 0 以外の抽出で待機型抽出が成立する
- [ ] `ReinforcementWaveId` を設定した抽出で増援イベントを発火できる

---

## §21-1 商人 DataTable / Trust 解放構造

### 概要
BX は固定拠点ではなく商人ネットワーク方式を採用するため、商人の基本定義と Trust 解放内容をデータで管理する必要がある。  
本章は `DT_BX_Merchants` と `DT_BX_MerchantTrustRewards` の 2 テーブル構造を定義する。

### 仕様
- 商人マスタ DataTable 名は `DT_BX_Merchants` とする。
- Trust 解放 DataTable 名は `DT_BX_MerchantTrustRewards` とする。
- 行構造体名はそれぞれ `FBXMerchantTableRow`, `FBXMerchantTrustRewardRow` とする。
- 商人主キーは `MerchantId` とし、既存 5 商人（ミナト / サワベ / クロガネ / ヨシナガ / アマギ）を固定とする。
- Trust しきい値は商人ごとに持てるようにする。
- 商人ごとの売買補正、解放タブ、初期接触フラグ、ホーム地域を定義する。
- Trust 解放は「必要レベル + 任意の依頼条件 + 任意のストーリーフラグ」で成立する。
- 解放種別はアイテムプール、サービス、再開地点、ディスカウント、依頼、会話、抽出支援を最低限含む。
- `RewardValueInt` と `RewardValueFloat` を併用し、インベントリ枠増加や割引率など可変値を扱えるようにする。
- 商人固有短文は UI と会話の両方で使えるよう `Tagline` を持つ。
- 商人画像は `PortraitTexture` 参照で管理する。

### データ構造

#### §21-1-1 商人マスタ `FBXMerchantTableRow`

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `MerchantId` | `FName` | ✅ | 主キー。例: `merchant_minato` |
| `DisplayName` | `FText` | ✅ | 表示名 |
| `MerchantType` | `ENUM_BX_MerchantType` | ✅ | 商人種別 |
| `HomeRegionId` | `FName` | ✅ | 主担当地域 ID |
| `Tagline` | `FText` | ✅ | ハブ表示用短文 |
| `Description` | `FText` |  | 商人説明文 |
| `InitialTrustLevel` | `int32` | ✅ | 初期 Trust レベル |
| `InitialTrustPoints` | `int32` | ✅ | 初期 Trust ポイント |
| `Trust2Threshold` | `int32` | ✅ | Trust2 到達値 |
| `Trust3Threshold` | `int32` | ✅ | Trust3 到達値 |
| `Trust4Threshold` | `int32` | ✅ | Trust4 到達値 |
| `BuyPriceMultiplier` | `float` | ✅ | プレイヤーが買う時の価格補正 |
| `SellPriceMultiplier` | `float` | ✅ | プレイヤーが売る時の価格補正 |
| `PortraitTexture` | `TSoftObjectPtr<UTexture2D>` |  | 顔 / アイコン |
| `MerchantHubWidgetClass` | `TSoftClassPtr<UUserWidget>` |  | 固有補助 Widget |
| `DefaultTradePoolId` | `FName` |  | 初期販売プール ID |
| `DefaultQuestPoolId` | `FName` |  | 初期依頼プール ID |
| `DefaultRespawnPoolId` | `FName` |  | 初期再開地点プール ID |
| `StoryFlagRequiredForContact` | `FName` |  | 接触に必要なストーリーフラグ |
| `bCanUnlockRespawnNodes` | `bool` | ✅ | 再開地点解放を持つか |
| `bCanProvideMedicalService` | `bool` | ✅ | 治療系サービスを持つか |
| `bCanProvideTravelService` | `bool` | ✅ | 移送 / 抽出支援を持つか |

#### §21-1-2 Trust 解放 `FBXMerchantTrustRewardRow`

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `RewardId` | `FName` | ✅ | 主キー |
| `MerchantId` | `FName` | ✅ | 対象商人 ID |
| `RequiredTrustLevel` | `int32` | ✅ | 必要 Trust レベル |
| `RequiredQuestId` | `FName` |  | 必要依頼 ID |
| `RequiredStoryFlag` | `FName` |  | 必要ストーリーフラグ |
| `RewardType` | `ENUM_BX_MerchantRewardType` | ✅ | 解放種別 |
| `UnlockTargetId` | `FName` | ✅ | 解放対象 ID |
| `RewardValueInt` | `int32` |  | 整数値報酬 |
| `RewardValueFloat` | `float` |  | 小数値報酬 |
| `UIMessage` | `FText` | ✅ | 解放時表示メッセージ |
| `bVisibleBeforeUnlock` | `bool` | ✅ | 未解放時に予告表示するか |
| `PriorityOrder` | `int32` | ✅ | UI 並び順 |

### CSV サンプル(該当する場合)

```csv
MerchantId,DisplayName,MerchantType,HomeRegionId,Tagline,Description,InitialTrustLevel,InitialTrustPoints,Trust2Threshold,Trust3Threshold,Trust4Threshold,BuyPriceMultiplier,SellPriceMultiplier,PortraitTexture,MerchantHubWidgetClass,DefaultTradePoolId,DefaultQuestPoolId,DefaultRespawnPoolId,StoryFlagRequiredForContact,bCanUnlockRespawnNodes,bCanProvideMedicalService,bCanProvideTravelService
merchant_minato,"古物商 ミナト",Trader,kansai_industrial_01,"持ち帰った物にだけ価値がある","General salvage merchant",1,0,100,260,520,1.00,1.00,"/Game/BX/UI/MerchantHub/Icons/T_BX_Portrait_Minato.T_BX_Portrait_Minato",,tradepool_minato_t1,questpool_minato_t1,respawnpool_minato,story_contact_minato,true,false,false
merchant_sawabe,"闇医者 サワベ",Medic,kansai_industrial_01,"死にきる前なら、まだ手はある","Underground medical support",1,0,100,260,520,1.08,0.95,"/Game/BX/UI/MerchantHub/Icons/T_BX_Portrait_Sawabe.T_BX_Portrait_Sawabe",,tradepool_sawabe_t1,questpool_sawabe_t1,respawnpool_sawabe,story_contact_sawabe,true,true,false
merchant_kurogane,"解体屋 クロガネ",Mechanic,kansai_industrial_01,"価値まで死ぬとは限らない","Repair and dismantle specialist",1,0,100,260,520,1.02,1.05,"/Game/BX/UI/MerchantHub/Icons/T_BX_Portrait_Kurogane.T_BX_Portrait_Kurogane",,tradepool_kurogane_t1,questpool_kurogane_t1,respawnpool_kurogane,story_contact_kurogane,true,false,false
merchant_yoshinaga,"運び屋 ヨシナガ",Courier,kansai_industrial_01,"拾うな。運べ","Transport and route contact",1,0,100,260,520,1.04,0.98,"/Game/BX/UI/MerchantHub/Icons/T_BX_Portrait_Yoshinaga.T_BX_Portrait_Yoshinaga",,tradepool_yoshinaga_t1,questpool_yoshinaga_t1,respawnpool_yoshinaga,story_contact_yoshinaga,true,false,true
merchant_amagi,"情報屋 アマギ",Informant,kansai_industrial_01,"価値は物にない。順番にある","Intel broker",1,0,100,260,520,1.10,1.10,"/Game/BX/UI/MerchantHub/Icons/T_BX_Portrait_Amagi.T_BX_Portrait_Amagi",,tradepool_amagi_t1,questpool_amagi_t1,respawnpool_amagi,story_contact_amagi,true,false,false
```

```csv
RewardId,MerchantId,RequiredTrustLevel,RequiredQuestId,RequiredStoryFlag,RewardType,UnlockTargetId,RewardValueInt,RewardValueFloat,UIMessage,bVisibleBeforeUnlock,PriorityOrder
reward_minato_storage_01,merchant_minato,2,quest_minato_route_02,,StorageSlots,merchant_storage_minato,10,0.0,"ミナトの預かり在庫枠が 10 増加",true,10
reward_sawabe_respawn_01,merchant_sawabe,2,quest_sawabe_medical_02,,RespawnNode,respawn_clinic_backroom,0,0.0,"診療所裏室が再開地点として解放",true,20
reward_kurogane_repair_02,merchant_kurogane,3,quest_kurogane_blueprint_01,,Service,service_repair_high,0,0.0,"高品質修理サービス解放",true,30
reward_yoshinaga_extract_01,merchant_yoshinaga,2,quest_yoshinaga_carry_02,,ExtractSupport,extract_courier_pickup,0,0.0,"運び屋回収便が利用可能になった",true,40
reward_amagi_discount_01,merchant_amagi,4,,story_tokyo_outerline,Discount,tradepool_amagi_t4,0,0.12,"アマギの情報取引が 12% 割引",false,50
```

### Blueprint / C++ 役割分担

- **C++ 側**:
  - `FBXMerchantTableRow`, `FBXMerchantTrustRewardRow` 定義
  - `UBXMerchantNetworkSubsystem` で商人状態計算、Trust 閾値判定、解放適用
  - SaveGame と商人ネットワーク状態同期
- **Blueprint 側**:
  - 商人一覧 UI、解放演出、会話表示
  - 商人ごとの固有 Widget 差し替え
  - Trust 上昇アニメーション
- **アセット配置先**:
  - C++: `Source/BX/Public/Data/Merchants/FBXMerchantTableRow.h`
  - C++: `Source/BX/Public/Data/Merchants/FBXMerchantTrustRewardRow.h`
  - C++: `Source/BX/Public/Systems/Merchant/BXMerchantNetworkSubsystem.h`
  - DataTable: `Content/BX/Data/Common/Merchants/DT_BX_Merchants`
  - DataTable: `Content/BX/Data/Common/Merchants/DT_BX_MerchantTrustRewards`
  - CSV ソース: `DataSource/Merchants/DT_BX_Merchants.csv`
  - CSV ソース: `DataSource/Merchants/DT_BX_MerchantTrustRewards.csv`

### UI

```text
┌──────────────────────────────────────────────┐
│ 商人: 情報屋 アマギ                          │
│ Trust: Lv3  275 / 520                       │
├──────────────────────────────────────────────┤
│ 次の解放候補                                 │
│ ・Lv4: 高危険ルート詳細                      │
│ ・Lv4: 情報取引割引                          │
│ ・条件: 東京外縁ストーリーフラグ必要         │
└──────────────────────────────────────────────┘
```

### 関連章
- §16-3
- §29-3
- §29-4

### 受け入れ条件
- [ ] `DT_BX_Merchants` に既存 5 商人が登録されている
- [ ] 商人ごとの Trust しきい値をデータで変更できる
- [ ] `DT_BX_MerchantTrustRewards` から再開地点 / サービス / 割引を解放できる
- [ ] `RequiredQuestId` と `RequiredStoryFlag` の両方を条件に使える
- [ ] 商人ハブで次の解放候補を表示できる

---

## §29-4 商人ハブの取引画面

### 概要
`WBP_BX_MerchantTrade` は、プレイヤー所持品・商人販売品・価格・比較情報を表示し、弾薬通貨経済を回すための取引画面である。  
本章は、買う / 売る / 数量指定 / 比較表示 / 価格プレビューの UI と ViewModel を定義する。

### 仕様
- Widget 名は `WBP_BX_MerchantTrade` とする。
- 画面は「販売品一覧」「プレイヤー所持品一覧」「詳細パネル」「取引確認バー」の 4 領域で構成する。
- 買い物価格は `BasePriceCredits * MerchantBuyPriceMultiplier * (1 ± PriceVarianceRatio)` を用いる。
- 売却価格は `BasePriceCredits * MerchantSellPriceMultiplier` を基準とし、耐久・汚損・カテゴリ補正を後乗せする。
- 取引数量は 1 / 5 / 最大 のプリセットと、スライダ入力を持つ。
- クレジット不足時は購入ボタンを無効化する。
- `bIsTradeable=false` アイテムは売却リストに表示しない。
- 武器は比較対象装備があれば、反動・重量・ADS・価格を比較表示する。
- 紙幣コレクター系アイテムは通常商人では売れず、`CollectorSeriesId` がある場合にのみコレクター商人で特別価値を表示してよい。
- 売却プレビューでは「取引後クレジット」を常時表示する。
- 取引確定は即時でもよいが、高額品は確認ダイアログを挟んでよい。

### データ構造

#### §29-4-1 取引行 ViewModel

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `ItemId` | `FName` | ✅ | アイテム ID |
| `DisplayName` | `FText` | ✅ | 表示名 |
| `QuantityOwned` | `int32` | ✅ | 所持数 |
| `UnitPriceCredits` | `int32` | ✅ | 現在単価 |
| `TotalPriceCredits` | `int32` | ✅ | 現在選択数量での総額 |
| `bCanBuy` | `bool` | ✅ | 購入可能か |
| `bCanSell` | `bool` | ✅ | 売却可能か |
| `bInsufficientCredits` | `bool` | ✅ | クレジット不足か |
| `DurabilityRatio` | `float` |  | 耐久割合（0.0〜1.0） |
| `CompareDeltaDamage` | `float` |  | 装備比較差分ダメージ |
| `CompareDeltaWeightKg` | `float` |  | 装備比較差分重量 |
| `CompareDeltaADSTimeSec` | `float` |  | 装備比較差分 ADS 時間 |
| `IconTexture` | `TSoftObjectPtr<UTexture2D>` |  | アイコン |

#### §29-4-2 取引セッション ViewModel

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `MerchantId` | `FName` | ✅ | 対象商人 ID |
| `PlayerCurrentCredits` | `int32` | ✅ | 現在所持クレジット |
| `ProjectedCreditsAfterTrade` | `int32` | ✅ | 取引後想定クレジット |
| `CurrentTab` | `ENUM_BX_TradeTab` | ✅ | Buy / Sell / Compare |
| `SelectedItemId` | `FName` |  | 現在選択アイテム |
| `SelectedQuantity` | `int32` | ✅ | 選択数量 |
| `MerchantBuyPriceMultiplier` | `float` | ✅ | 購入価格補正 |
| `MerchantSellPriceMultiplier` | `float` | ✅ | 売却価格補正 |
| `TransactionWarningText` | `FText` |  | 注意文 |
| `RecentTradeLog` | `TArray<FText>` |  | 最近の取引ログ |

### Blueprint / C++ 役割分担

- **C++ 側**:
  - `UBXMerchantTradeSubsystem` で価格算出、数量確定、取引適用
  - ViewModel 用構造体定義
  - 所持クレジット・在庫・商人在庫の同期
- **Blueprint 側**:
  - `WBP_BX_MerchantTrade` のレイアウト
  - アイテム選択、数量スライダ、確認ダイアログ
  - 比較矢印、赤字 / 緑字表示
- **アセット配置先**:
  - C++: `Source/BX/Public/UI/MerchantHub/BXMerchantTradeSubsystem.h`
  - C++: `Source/BX/Public/UI/MerchantHub/FBXMerchantTradeViewModels.h`
  - Widget: `Content/BX/UI/MerchantHub/WBP_BX_MerchantTrade`
  - 子 Widget: `Content/BX/UI/MerchantHub/Components/WBP_BX_TradeRow`
  - アイコン: `Content/BX/UI/MerchantHub/Icons/`

### UI

```text
┌─────────────────────────────────────────────────────────────────────┐
│ 取引 [Buy] [Sell] [Compare]      所持 Credits: 1840 → 640         │
├──────────────────────┬──────────────────────┬───────────────────────┤
│ 商人販売品             │ プレイヤー所持品        │ 詳細 / 比較             │
│ > Bandage     300     │   9mm FMJ x48         │ Bandage                │
│   Tourniquet  900     │   Old Yen Note x12    │ 単価: 300              │
│   Ration      500     │   HG-9A (82%)         │ 数量: 4                │
│   9mm FMJ     1200    │                       │ 総額: 1200             │
├──────────────────────┴──────────────────────┴───────────────────────┤
│ [ - ] [1] [5] [MAX] [ + ]      [購入確定]  [売却確定]               │
└─────────────────────────────────────────────────────────────────────┘
```

### 関連章
- §15-1
- §21-1
- §29-3

### 受け入れ条件
- [ ] 販売品一覧とプレイヤー所持品一覧を同画面で表示できる
- [ ] 数量変更に応じて `ProjectedCreditsAfterTrade` が更新される
- [ ] クレジット不足時は購入確定ボタンが無効化される
- [ ] `bIsTradeable=false` アイテムは売却リストに表示されない
- [ ] 武器選択時に比較差分（重量 / ADS / ダメージ）を表示できる


---

## §18-3 抽出ポイント Actor 基底仕様

### 概要
`ABXExtractPointBase` は、ワールド上の抽出地点で共通に必要な「接触判定」「条件評価」「待機タイマー」「中断条件」「成功通知」を提供する C++ 基底 Actor である。  
各抽出地点の見た目や演出は Blueprint 派生で差し替えるが、抽出ルール評価とゲーム進行反映はこの基底仕様に統一する。

### 仕様
- 基底クラス名は `ABXExtractPointBase` とする。
- Blueprint 派生アセット命名は `BP_BX_ExtractPoint_<Region>_<Type>` とする。
- 抽出ルールは `§18-2` の `DT_BX_ExtractRules` と `ExtractId` で接続する。
- Actor は最低限以下のコンポーネントを持つ。
  - `USceneComponent` Root
  - `UStaticMeshComponent` または `UNiagaraComponent` 等の視覚コンポーネント
  - `UBoxComponent` または `USphereComponent` の侵入判定
  - 任意の `UAudioComponent`
- プレイヤーが判定範囲に入ると、現在の `ExtractId` に対応する条件を再評価する。
- 条件が未達成の場合は UI に未達条件を表示するが、タイマーは開始しない。
- 条件達成かつプレイヤーが抽出開始入力を行った場合、`CountdownTimeSec` に従って抽出タイマーを開始する。
- タイマー進行中は `§18-2` の中断条件フラグに従って抽出をキャンセルできる。
- 抽出成功時は `GS_BX_Raid` または同等のレイド状態管理へ成功イベントを通知する。
- 増援 Wave が設定されている場合、抽出開始時または成功直前にイベントを発火できる。
- `bOneWayOnly=true` の抽出は、一度成功した同レイド中は逆利用不可とする。
- EXリアルで無効な抽出は候補表示のみ、または完全非表示のどちらかを `MarkerVisibilityType` で選べるようにする。
- 抽出待機中の進捗は 0.0〜1.0 のノーマライズ値で Widget に通知する。
- ワールド配置の個別オフセットや待機中心点は Blueprint で微調整可能とする。

### データ構造

#### §18-3-1 Actor ランタイム状態

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `ExtractId` | `FName` | ✅ | `DT_BX_ExtractRules` の参照キー |
| `CurrentProgressRatio` | `float` | ✅ | 抽出進捗（0.0〜1.0） |
| `bIsPlayerInsideTrigger` | `bool` | ✅ | プレイヤーが判定内にいるか |
| `bIsExtractionRunning` | `bool` | ✅ | 抽出タイマー進行中か |
| `bIsExtractionLocked` | `bool` | ✅ | このレイド中にロック状態か |
| `bHasSatisfiedConditions` | `bool` | ✅ | 現在条件を満たしているか |
| `LastFailedReasonTags` | `FGameplayTagContainer` |  | 最後の失敗理由タグ群 |
| `CountdownRemainSec` | `float` | ✅ | 残り待機時間（秒） |
| `CachedRuleRowName` | `FName` | ✅ | キャッシュ済みルール行名 |
| `TriggerRadiusOverride` | `float` |  | 判定範囲上書き値（m） |

#### §18-3-2 条件評価結果

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `bCanUseNow` | `bool` | ✅ | 現時点で使用可能か |
| `bMeetsKeyRequirement` | `bool` | ✅ | 鍵条件を満たすか |
| `bMeetsMerchantRequirement` | `bool` | ✅ | 商人条件を満たすか |
| `bMeetsQuestRequirement` | `bool` | ✅ | 依頼条件を満たすか |
| `bMeetsStoryRequirement` | `bool` | ✅ | ストーリー条件を満たすか |
| `bMeetsPowerRequirement` | `bool` | ✅ | 電源 / 制御条件を満たすか |
| `bMeetsAlertRequirement` | `bool` | ✅ | 警報条件を満たすか |
| `bMeetsWeightRequirement` | `bool` | ✅ | 重量条件を満たすか |
| `bMeetsTimeRequirement` | `bool` | ✅ | 時刻帯条件を満たすか |
| `FailureReasonTags` | `FGameplayTagContainer` |  | 未達理由タグ群 |

#### §18-3-3 抽出イベント通知

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `ExtractId` | `FName` | ✅ | 抽出地点 ID |
| `RegionId` | `FName` | ✅ | 地域 ID |
| `bWasSuccessful` | `bool` | ✅ | 成功 / 失敗 |
| `StartTimeSec` | `float` | ✅ | 開始時刻（レイド経過秒） |
| `EndTimeSec` | `float` | ✅ | 終了時刻（レイド経過秒） |
| `CancelReasonTag` | `FGameplayTag` |  | 中断理由 |
| `TriggeredWaveId` | `FName` |  | 発火した増援 Wave ID |

### Blueprint / C++ 役割分担

- **C++ 側**:
  - `ABXExtractPointBase` クラス定義
  - 条件評価・タイマー進行・中断判定
  - レイド状態管理への成功 / 中断通知
  - `§18-2` の DataTable 読み込みとキャッシュ
- **Blueprint 側**:
  - 見た目メッシュ、ライト、ロープ、車両、ゲートなどの個別演出
  - 開始時 / 成功時 / 失敗時のサウンドや Niagara
  - マップ個別の判定範囲微調整
- **アセット配置先**:
  - C++: `Source/BX/Public/World/Extract/ABXExtractPointBase.h`
  - C++: `Source/BX/Private/World/Extract/ABXExtractPointBase.cpp`
  - Blueprint 基底派生: `Content/BX/World/Extract/`
  - 共通 Widget: `Content/BX/UI/HUD/Extract/`

### UI

```text
┌──────────────────────────────────────┐
│ 抽出進行中: East Checkpoint          │
│ Progress: ███████░░░ 72%             │
│ 中断条件: 被弾 / 範囲外移動          │
│ 警告: 増援接近                        │
└──────────────────────────────────────┘
```

### 関連章
- §18-2
- §16-3
- §29-3

### 受け入れ条件
- [ ] `ABXExtractPointBase` から Blueprint 派生抽出ポイントを作成できる
- [ ] `ExtractId` に対応する `DT_BX_ExtractRules` 行を読み込める
- [ ] 条件未達時はタイマーが開始しない
- [ ] 待機中に被弾または範囲外移動で中断できる
- [ ] 抽出成功時にレイド成功イベントを通知できる

---

## §21-2 依頼 DataTable と進行構造

### 概要
BX の依頼は、単なる報酬取得ではなく、商人 Trust、再開地点解放、ストーリー導線、抽出理解を成立させる中心システムである。  
本章は依頼のマスタ DataTable、進行保存構造、依頼タイプごとの達成判定を定義する。

### 仕様
- DataTable 名は `DT_BX_Quests` とする。
- 行構造体名は `FBXQuestTableRow` とする。
- 主キーは `QuestId` とする。
- 依頼種別は `ENUM_BX_QuestType` で管理する。
- 依頼は最低限以下のタイプに対応する。
  - Collect
  - Deliver
  - Survive
  - Extract
  - Discover
  - Kill
  - UseExtract
  - TurnInCollectorItem
- 依頼の開始条件は「商人接触」「Trust」「依頼前提」「ストーリーフラグ」の組み合わせで定義する。
- 依頼の進捗は `CurrentCount / TargetCount` を基本とし、必要に応じて複数条件をタグ配列で補完する。
- 依頼完了報酬はクレジット、Trust、解放 ID、アイテム、再開地点、ストーリーフラグ付与に対応する。
- 依頼失敗は一部のみ対応し、基本は未達成継続扱いとする。
- 失敗型依頼には `FailureRuleTag` を設定する。
- 依頼 UI 表示順と重要度は `PriorityOrder` と `bIsMainQuestLike` で制御する。
- 周回防止が必要な依頼は `bIsRepeatable=false` とする。
- 商人ハブは `ActiveQuests`, `CompletedQuests`, `FailedQuests` を `§16-3` の Save 構造と同期する。

### データ構造

#### §21-2-1 依頼マスタ `FBXQuestTableRow`

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `QuestId` | `FName` | ✅ | 主キー |
| `DisplayName` | `FText` | ✅ | 依頼名 |
| `MerchantId` | `FName` | ✅ | 依頼主商人 ID |
| `QuestType` | `ENUM_BX_QuestType` | ✅ | 依頼種別 |
| `Description` | `FText` | ✅ | 依頼説明 |
| `TargetItemId` | `FName` |  | 対象アイテム ID |
| `TargetEnemyId` | `FName` |  | 対象敵 ID |
| `TargetRegionId` | `FName` |  | 対象地域 ID |
| `TargetExtractId` | `FName` |  | 対象抽出 ID |
| `TargetStoryFlag` | `FName` |  | 発見 / 到達対象フラグ |
| `TargetCount` | `int32` | ✅ | 必要数 |
| `StartRequiredTrustLevel` | `int32` | ✅ | 開始に必要な Trust |
| `StartRequiredQuestId` | `FName` |  | 開始前提依頼 |
| `StartRequiredStoryFlag` | `FName` |  | 開始前提ストーリーフラグ |
| `RewardCredits` | `int32` | ✅ | 報酬クレジット |
| `RewardTrustPoints` | `int32` | ✅ | 報酬 Trust |
| `RewardUnlockId` | `FName` |  | 解放対象 ID |
| `RewardItemId` | `FName` |  | 報酬アイテム ID |
| `RewardItemCount` | `int32` |  | 報酬アイテム数 |
| `RewardStoryFlag` | `FName` |  | 完了時付与フラグ |
| `FailureRuleTag` | `FGameplayTag` |  | 失敗条件タグ |
| `bIsRepeatable` | `bool` | ✅ | 繰り返し受注可能か |
| `bIsMainQuestLike` | `bool` | ✅ | UI 上で主要扱いか |
| `PriorityOrder` | `int32` | ✅ | 並び順 |
| `UIHintText` | `FText` |  | UI 補助文 |

#### §21-2-2 ランタイム進捗 `FBXQuestRuntimeProgress`

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `QuestId` | `FName` | ✅ | 依頼 ID |
| `CurrentCount` | `int32` | ✅ | 現在進捗数 |
| `TargetCount` | `int32` | ✅ | 必要数 |
| `bIsAccepted` | `bool` | ✅ | 受注済みか |
| `bIsCompleted` | `bool` | ✅ | 完了済みか |
| `bIsFailed` | `bool` | ✅ | 失敗済みか |
| `AcceptedAtRaidCount` | `int32` |  | 受注時レイド数 |
| `LastUpdatedAtRegionId` | `FName` |  | 最終更新地域 ID |
| `ProgressTags` | `FGameplayTagContainer` |  | 補助進捗タグ |
| `TrackedValueFloat` | `float` |  | 距離 / 重量などの補助値 |

### CSV サンプル(該当する場合)

```csv
QuestId,DisplayName,MerchantId,QuestType,Description,TargetItemId,TargetEnemyId,TargetRegionId,TargetExtractId,TargetStoryFlag,TargetCount,StartRequiredTrustLevel,StartRequiredQuestId,StartRequiredStoryFlag,RewardCredits,RewardTrustPoints,RewardUnlockId,RewardItemId,RewardItemCount,RewardStoryFlag,FailureRuleTag,bIsRepeatable,bIsMainQuestLike,PriorityOrder,UIHintText
quest_minato_route_02,"搬出記録",merchant_minato,Collect,"港湾事務所から搬出記録を回収せよ",item_port_record_01,,kansai_industrial_01,,,1,1,quest_minato_route_01,,1800,35,reward_minato_storage_01,,,0,story_minato_route_found,,false,true,10,"港区南側の事務所を探せ"
quest_sawabe_medical_02,"診療所の裏口",merchant_sawabe,Discover,"研究搬送センター周辺の医療記録を持ち帰れ",, ,research_center_01,,story_research_medlog,1,1,quest_sawabe_medical_01,,2200,35,reward_sawabe_respawn_01,med_trauma_kit_01,1,story_sawabe_backroom,,false,true,20,"研究施設の記録端末に注意"
quest_yoshinaga_carry_02,"落とさず運べ",merchant_yoshinaga,Deliver,"指定貨物を損傷なく持ち帰れ",item_transport_case_01,,kansai_industrial_01,extract_courier_pickup,,1,1,quest_yoshinaga_carry_01,,2500,35,reward_yoshinaga_extract_01,item_pickup_token_01,1,story_courier_pickup_unlocked,Quest.Fail.ItemBroken,false,true,30,"貨物破損で失敗"
quest_amagi_collector_01,"紙切れの値段",merchant_amagi,TurnInCollectorItem,"珍しい年号の旧円紙幣を 3 枚納品せよ",cash_note_rare,,, ,,,3,1,,story_contact_amagi,1600,20,,ammo_9mm_fmj_pack_01,1,story_collector_intro,,true,false,40,"ゾロ目や切り番の紙幣が高価値"
```

### Blueprint / C++ 役割分担

- **C++ 側**:
  - `FBXQuestTableRow`, `FBXQuestRuntimeProgress` 定義
  - `UBXQuestSubsystem` で受注、進捗更新、完了判定、報酬配布
  - SaveGame との同期
  - 依頼タイプごとの進捗ハンドラ
- **Blueprint 側**:
  - 商人ハブ依頼一覧 UI
  - 依頼受注 / 完了演出
  - マップ上の依頼マーカー表示
- **アセット配置先**:
  - C++: `Source/BX/Public/Data/Quests/FBXQuestTableRow.h`
  - C++: `Source/BX/Public/Systems/Quest/BXQuestSubsystem.h`
  - DataTable: `Content/BX/Data/Common/Quests/DT_BX_Quests`
  - UI: `Content/BX/UI/MerchantHub/Quests/`
  - CSV ソース: `DataSource/Quests/DT_BX_Quests.csv`

### UI

```text
┌──────────────────────────────────────────────┐
│ 依頼: 落とさず運べ                           │
├──────────────────────────────────────────────┤
│ 依頼主: 運び屋 ヨシナガ                      │
│ 種別: Deliver                                │
│ 進捗: 0 / 1                                  │
│ 条件: 指定貨物を損傷なく Courier Pickup へ   │
│ 失敗条件: 貨物破損                            │
│ 報酬: 2500 Credits / Trust +35 / Pickup 解放 │
└──────────────────────────────────────────────┘
```

### 関連章
- §16-3
- §21-1
- §29-3

### 受け入れ条件
- [ ] `DT_BX_Quests` に 12 件以上のサンプル依頼が登録されている
- [ ] Collect / Deliver / Discover / UseExtract の 4 種以上で進捗更新できる
- [ ] 完了時にクレジット / Trust / 解放 ID を付与できる
- [ ] 失敗条件タグ付き依頼で `bIsFailed=true` に遷移できる
- [ ] 商人ハブで依頼の受注・進捗・完了状態を表示できる

---

## §15-3 医療アイテム詳細テーブル

### 概要
BX の医療アイテムは「何でも治る万能回復」ではなく、止血・大出血停止・骨折処置・疼痛軽減・水分補給・Neuro Critical 延命という役割分担で成立する。  
本章は `DT_BX_MedicalItems` の詳細行構造と、使用アクションとの接続を定義する。

### 仕様
- DataTable 名は `DT_BX_MedicalItems` とする。
- 行構造体名は `FBXMedicalItemRow` とする。
- `DT_BX_Items` の `ItemId` と 1:1 または多対1 で接続し、医療詳細を別テーブルで管理する。
- 医療カテゴリは最低限以下に分ける。
  - Bandage
  - Tourniquet
  - TraumaKit
  - Splint
  - Painkiller
  - Hydration
  - Surgery
  - NeuroSaver
- 使用時間は `UseTimeSec` で代表値を持つ。
- 使用中断条件は被弾 / ダッシュ / 武器使用を基準とする。
- `RestoresHP` は純粋回復量、`CuresStatusIds` は除去対象異常配列で定義する。
- Neuro Critical 対応アイテムは `bCanResolveNeuroCritical=true` とし、通常治療とは別に扱う。
- 使用後クールダウンが必要なものは `CooldownSec` を持つ。
- 一時的効果（鎮痛など）は `AppliedBuffStatusId` で管理する。
- レイド中持込制限が必要な高級医療は `MaxCarryPerRaid` を持つ。
- 旧円紙幣 / 依頼品など非医療アイテムは本テーブルに含めない。

### データ構造

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `MedicalItemId` | `FName` | ✅ | 主キー |
| `ItemId` | `FName` | ✅ | `DT_BX_Items` の対象アイテム ID |
| `MedicalType` | `ENUM_BX_MedicalType` | ✅ | 医療カテゴリ |
| `DisplayName` | `FText` | ✅ | 表示名 |
| `UseTimeSec` | `float` | ✅ | 使用時間（秒） |
| `CooldownSec` | `float` | ✅ | 再使用クールダウン（秒） |
| `MaxCharges` | `int32` | ✅ | 1 個あたり最大使用回数 |
| `RestoresHP` | `float` | ✅ | HP 回復量 |
| `RestoresHydration` | `float` | ✅ | 水分回復量 |
| `RestoresEnergy` | `float` | ✅ | 空腹回復量 |
| `PainReliefRatio` | `float` | ✅ | 疼痛軽減係数 |
| `bStopsSmallBleed` | `bool` | ✅ | 小出血を止めるか |
| `bStopsLargeBleed` | `bool` | ✅ | 大出血を止めるか |
| `bFixesFracture` | `bool` | ✅ | 骨折を治療するか |
| `bCanResolveNeuroCritical` | `bool` | ✅ | Neuro Critical を解除できるか |
| `CuresStatusIds` | `TArray<FName>` |  | 解除対象状態異常 ID 配列 |
| `AppliedBuffStatusId` | `FName` |  | 使用後付与するバフ / デバフ ID |
| `bCancelOnDamage` | `bool` | ✅ | 被弾で中断するか |
| `bCancelOnSprint` | `bool` | ✅ | ダッシュで中断するか |
| `bCancelOnWeaponUse` | `bool` | ✅ | 武器使用で中断するか |
| `MaxCarryPerRaid` | `int32` | ✅ | レイド持込上限（0 なら無制限） |
| `UseActionId` | `FName` | ✅ | 使用アクション ID |
| `UseMontage1P` | `TSoftObjectPtr<UAnimMontage>` |  | 一人称使用モンタージュ |
| `UseSound` | `TSoftObjectPtr<USoundBase>` |  | 使用音 |
| `Description` | `FText` |  | 説明文 |

### CSV サンプル(該当する場合)

```csv
MedicalItemId,ItemId,MedicalType,DisplayName,UseTimeSec,CooldownSec,MaxCharges,RestoresHP,RestoresHydration,RestoresEnergy,PainReliefRatio,bStopsSmallBleed,bStopsLargeBleed,bFixesFracture,bCanResolveNeuroCritical,CuresStatusIds,AppliedBuffStatusId,bCancelOnDamage,bCancelOnSprint,bCancelOnWeaponUse,MaxCarryPerRaid,UseActionId,UseMontage1P,UseSound,Description
med_bandage_detail_01,med_bandage_01,Bandage,"Bandage",2.2,0.5,2,0,0,0,0.0,true,false,false,false,"status_small_bleed",,true,true,true,0,use_bandage,"/Game/BX/Animations/Medical/AM_BX_Use_Bandage_1P.AM_BX_Use_Bandage_1P","/Game/BX/Audio/Medical/SFX_BX_Use_Bandage.SFX_BX_Use_Bandage","Stops small bleed"
med_tourniquet_detail_01,med_tourniquet_01,Tourniquet,"Tourniquet",3.0,1.0,1,0,0,0,0.0,false,true,false,false,"status_large_bleed",,true,true,true,0,use_tourniquet,"/Game/BX/Animations/Medical/AM_BX_Use_Tourniquet_1P.AM_BX_Use_Tourniquet_1P","/Game/BX/Audio/Medical/SFX_BX_Use_Tourniquet.SFX_BX_Use_Tourniquet","Stops large bleed"
med_splint_detail_01,med_splint_01,Splint,"Splint",6.0,1.0,1,0,0,0,0.0,false,false,true,false,"status_leg_fracture|status_arm_fracture",,true,true,true,0,use_splint,"/Game/BX/Animations/Medical/AM_BX_Use_Splint_1P.AM_BX_Use_Splint_1P","/Game/BX/Audio/Medical/SFX_BX_Use_Splint.SFX_BX_Use_Splint","Fixes fractures"
med_painkiller_detail_01,med_painkiller_01,Painkiller,"Painkiller",2.0,30.0,4,0,0,0,0.55,false,false,false,false,,status_painkiller_buff,false,true,true,0,use_painkiller,"/Game/BX/Animations/Medical/AM_BX_Use_Painkiller_1P.AM_BX_Use_Painkiller_1P","/Game/BX/Audio/Medical/SFX_BX_Use_Painkiller.SFX_BX_Use_Painkiller","Reduces pain temporarily"
med_n15_detail_01,med_n15_01,NeuroSaver,"N-15",1.8,0.0,1,0,0,0,0.0,false,false,false,true,"status_neuro_critical",status_neuro_aftershock,true,true,true,1,use_n15,"/Game/BX/Animations/Medical/AM_BX_Use_N15_1P.AM_BX_Use_N15_1P","/Game/BX/Audio/Medical/SFX_BX_Use_N15.SFX_BX_Use_N15","Resolves Neuro Critical at severe cost"
```

### Blueprint / C++ 役割分担

- **C++ 側**:
  - `FBXMedicalItemRow` 定義
  - `UBXMedicalUseSubsystem` で使用判定、状態異常解除、使用中断、クールダウン管理
  - `§15-2` の状態異常テーブルと接続
- **Blueprint 側**:
  - 医療使用モンタージュ再生
  - 使用中プログレス UI
  - 視界エフェクトや手元演出
- **アセット配置先**:
  - C++: `Source/BX/Public/Data/Medical/FBXMedicalItemRow.h`
  - C++: `Source/BX/Public/Systems/Medical/BXMedicalUseSubsystem.h`
  - DataTable: `Content/BX/Data/Common/Medical/DT_BX_MedicalItems`
  - Animation: `Content/BX/Animations/Medical/`
  - Audio: `Content/BX/Audio/Medical/`
  - CSV ソース: `DataSource/Medical/DT_BX_MedicalItems.csv`

### UI

```text
┌──────────────────────────────────────────────┐
│ 使用中: N-15                                 │
│ Progress: ██████░░░░ 61%                     │
│ 効果: Neuro Critical 解除                    │
│ 警告: 使用後に強い後遺症が発生               │
└──────────────────────────────────────────────┘
```

### 関連章
- §15-1
- §15-2
- §16-3

### 受け入れ条件
- [ ] `DT_BX_MedicalItems` に 8 件以上の医療詳細データがある
- [ ] Bandage / Tourniquet / Splint / Painkiller / N-15 の 5 種が個別挙動を持つ
- [ ] 被弾で中断する医療品としない医療品を分けて設定できる
- [ ] `bCanResolveNeuroCritical=true` の医療品でのみ Neuro Critical を解除できる
- [ ] `MaxCarryPerRaid` を使って N-15 の持込上限を 1 個に設定できる
