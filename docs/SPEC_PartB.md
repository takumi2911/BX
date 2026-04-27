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


## §23-4 プレイヤー基底クラス

### 概要

本章では、プレイヤーキャラクターの基底クラス `APlayerCharacterBase` を定義する。
本クラスは、`AC_BX_Inventory`、`AC_BX_StatusEffects`、`AC_BX_HealthBodyParts`、`AC_BX_WeaponHandler`、`AC_BX_PlayerInteraction`、`UBXMedicalUseSubsystem`、`UBXQuestSubsystem`、`UBXMerchantNetworkSubsystem` 等を統合する正本となる Pawn である。

### ASCII クラス階層図

```text
ACharacter
└─ APlayerCharacterBase
   └─ BP_BX_Player
```

### 仕様

* C++ クラス名は `APlayerCharacterBase` とする。
* Blueprint 派生は `BP_BX_Player` とする。
* 役割は以下を統合する。
  * 入力受付
  * カメラ・移動・姿勢制御
  * 各コンポーネント所有
  * 各 Subsystem との橋渡し
  * SaveGame との連携
* 個別ロジック(部位 HP、状態異常、インベントリ等)は本クラス側で持たず、コンポーネント側で持つ。
* 本クラスは「窓口」であり「正本」を直接持たない。

### 必須コンポーネント

| コンポーネント名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `Mesh1P` | `USkeletalMeshComponent*` | ✅ | 一人称メッシュ |
| `Mesh3P` | `USkeletalMeshComponent*` | ✅ | 三人称メッシュ(共有) |
| `CameraFirstPerson` | `UCameraComponent*` | ✅ | 一人称カメラ |
| `SpringArm` | `USpringArmComponent*` |  | 三人称用 |
| `CameraThirdPerson` | `UCameraComponent*` |  | 三人称カメラ |
| `InventoryComponent` | `UAC_BX_Inventory*` | ✅ | 所持品 |
| `StatusEffectsComponent` | `UAC_BX_StatusEffects*` | ✅ | 状態異常 |
| `HealthBodyPartsComponent` | `UAC_BX_HealthBodyParts*` | ✅ | 部位 HP |
| `WeaponHandlerComponent` | `UAC_BX_WeaponHandler*` | ✅ | 武器保持 |
| `PlayerInteractionComponent` | `UAC_BX_PlayerInteraction*` | ✅ | インタラクト窓口 |

### 入力アクション

| アクション名 | 用途 |
| --- | --- |
| `IA_BX_Move` | 移動 |
| `IA_BX_Look` | 視点回転 |
| `IA_BX_Jump` | ジャンプ |
| `IA_BX_Sprint` | スプリント |
| `IA_BX_Crouch` | しゃがみ |
| `IA_BX_Prone` | 伏せ |
| `IA_BX_LeanLeft` | 左リーン |
| `IA_BX_LeanRight` | 右リーン |
| `IA_BX_FirePrimary` | 主射撃 |
| `IA_BX_FireSecondary` | 副射撃 / ADS |
| `IA_BX_Reload` | リロード |
| `IA_BX_SwitchWeaponPrimary` | 主武器選択 |
| `IA_BX_SwitchWeaponSecondary` | 副武器選択 |
| `IA_BX_SwitchWeaponPistol` | ハンドガン選択 |
| `IA_BX_SwitchWeaponMelee` | 近接武器選択 |
| `IA_BX_Holster` | 武器収納 |
| `IA_BX_Interact` | インタラクト |
| `IA_BX_MedicalUse` | 医療使用 |
| `IA_BX_QuickSlot1` | クイックスロット1 |
| `IA_BX_QuickSlot2` | クイックスロット2 |
| `IA_BX_QuickSlot3` | クイックスロット3 |
| `IA_BX_QuickSlot4` | クイックスロット4 |
| `IA_BX_OpenInventory` | インベントリ画面 |
| `IA_BX_OpenMap` | マップ画面 |

### 視点・姿勢・移動状態

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `CurrentViewMode` | `EBXViewMode` | ✅ | First / Third |
| `CurrentLocomotionState` | `EBXLocomotionState` | ✅ | Idle / Walk / Run / Sprint / Crouch / Prone |
| `CurrentLeanState` | `EBXLeanState` | ✅ | None / Left / Right |
| `CurrentCombatState` | `EBXCombatState` | ✅ | None / Aiming / Firing / Reloading / Switching |
| `bIsMedicalUseActive` | `bool` | ✅ | 医療使用中 |
| `bIsInteractionActive` | `bool` | ✅ | インタラクト進行中 |
| `bIsExtractingActive` | `bool` | ✅ | 脱出進行中 |

### 状態フラグ

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `bAlive` | `bool` | ✅ | 生存中か |
| `bUnconscious` | `bool` | ✅ | 失神中か |
| `bIsInExtractZone` | `bool` | ✅ | 脱出ゾーン内か |
| `bInputLocked` | `bool` | ✅ | 入力ロック中か |
| `MovementInputLockTags` | `FGameplayTagContainer` |  | 移動ロックタグ |
| `CombatInputLockTags` | `FGameplayTagContainer` |  | 戦闘ロックタグ |

### HUD 公開フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `HudCarryWeightKg` | `float` | ✅ | 現在重量 |
| `HudCarryMaxWeightKg` | `float` | ✅ | 最大重量 |
| `HudCarryState` | `EBXCarryState` | ✅ | 重量状態 |
| `HudCurrentLanguage` | `EBXLanguage` | ✅ | 表示言語 |
| `HudActiveWeaponName` | `FText` |  | 装備武器名 |
| `HudActiveWeaponAmmoCurrent` | `int32` |  | 現在弾数 |
| `HudActiveWeaponAmmoMax` | `int32` |  | 最大弾数 |
| `HudCurrentExtractName` | `FText` |  | 現在脱出ポイント名 |
| `HudCurrentExtractRemainingSec` | `float` |  | 脱出残り時間 |

### Save 公開構造体(参照用)

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `SavedTransform` | `FTransform` | ✅ | 位置姿勢 |
| `SavedViewMode` | `EBXViewMode` | ✅ | 視点 |
| `SavedHealthState` | `FBXHealthState` | ✅ | 部位 HP 状態 |
| `SavedStatusEffectsState` | `FBXStatusEffectsState` | ✅ | 状態異常状態 |
| `SavedInventoryState` | `FBXInventoryState` | ✅ | 所持品状態 |
| `SavedWeaponState` | `FBXWeaponHandlerState` | ✅ | 武器状態 |

### C++ 関数シグネチャ

```cpp
UFUNCTION(BlueprintCallable, Category="BX|Player")
void InitializePlayerCharacter();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void BindAllInputActions(class UEnhancedInputComponent* InputComponent);

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_MoveTriggered(const FInputActionValue& Value);

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_LookTriggered(const FInputActionValue& Value);

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_JumpTriggered();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_SprintStarted();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_SprintCompleted();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_CrouchToggled();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_ProneToggled();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_LeanLeftStarted();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_LeanLeftCompleted();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_LeanRightStarted();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_LeanRightCompleted();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_FirePrimaryStarted();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_FirePrimaryCompleted();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_FireSecondaryTriggered();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_ReloadTriggered();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_SwitchWeaponSlot(EBXWeaponSlot WeaponSlot);

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_HolsterTriggered();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_InteractTriggered();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_MedicalUseTriggered();

UFUNCTION(BlueprintCallable, Category="BX|Player")
void Input_QuickSlotTriggered(int32 QuickSlotIndex);

UFUNCTION(BlueprintCallable, Category="BX|Player")
void SetViewMode(EBXViewMode NewViewMode);

UFUNCTION(BlueprintPure, Category="BX|Player")
EBXViewMode GetCurrentViewMode() const;

UFUNCTION(BlueprintPure, Category="BX|Player")
EBXLocomotionState GetCurrentLocomotionState() const;

UFUNCTION(BlueprintPure, Category="BX|Player")
EBXCombatState GetCurrentCombatState() const;

UFUNCTION(BlueprintCallable, Category="BX|Player")
void LockInput(FGameplayTag LockTag);

UFUNCTION(BlueprintCallable, Category="BX|Player")
void UnlockInput(FGameplayTag LockTag);

UFUNCTION(BlueprintPure, Category="BX|Player")
bool IsInputLocked() const;

UFUNCTION(BlueprintCallable, Category="BX|Player")
void NotifyDeathOrUnconscious();

UFUNCTION(BlueprintCallable, Category="BX|Player|Save")
void ExportPlayerSaveState(FBXPlayerSaveState& OutState) const;

UFUNCTION(BlueprintCallable, Category="BX|Player|Save")
void ImportPlayerSaveState(const FBXPlayerSaveState& InState);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Player")
void BP_OnViewModeChanged(EBXViewMode NewViewMode);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Player")
void BP_OnLocomotionStateChanged(EBXLocomotionState NewState);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Player")
void BP_OnCombatStateChanged(EBXCombatState NewState);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Player")
void BP_OnPlayerDeath();
```

### Blueprint / C++ 役割分担

* **C++ 側**:
  * Pawn 構造定義
  * 各コンポーネント所有・初期化
  * 入力受付窓口
  * 状態遷移と各コンポーネント呼び出し
  * Save / Load 連携
* **Blueprint 側**:
  * `BP_BX_Player` 派生
  * メッシュ・カメラ・SpringArm のレベル別調整
  * Anim Blueprint 接続
  * 入力 Mapping Context 割当
  * 状態演出
* **アセット配置先**:
  * `Source/BX/Public/Characters/APlayerCharacterBase.h`
  * `Source/BX/Private/Characters/APlayerCharacterBase.cpp`
  * `Content/BX/Blueprints/Player/BP_BX_Player`
  * `Content/BX/Input/IA_BX_*`

### 関連章

* §23-5
* §23-6
* §23-7
* §24-1
* §24-3
* §16-4

### 受け入れ条件

* [ ] `APlayerCharacterBase` が C++ で定義されている
* [ ] 必須コンポーネント 9 個が初期化されている
* [ ] 入力アクション群が `IA_BX_*` でバインドされている
* [ ] 視点・姿勢・移動・戦闘状態を保持できる
* [ ] HUD 公開フィールドを保持できる
* [ ] Save / Load の構造体に変換できる
* [ ] `GM_BX_Raid` から `BP_BX_Player` を Default Pawn として設定できる

---

## §23-5 武器保持コンポーネント

### 概要

本章では、プレイヤーが装備する武器スロットの保持・切替・装填・射撃モード管理・HUD 公開を担う `AC_BX_WeaponHandler` を定義する。
C++ クラス名は `UAC_BX_WeaponHandler`、アタッチ名は `AC_BX_WeaponHandler` とする。
本コンポーネントは武器個体の正本を持ち、`AC_BX_Inventory` の弾薬と連携する。

### ASCII クラス / 依存図

```text
UActorComponent
└─ UAC_BX_WeaponHandler
   └─ owned by APlayerCharacterBase

UAC_BX_WeaponHandler
├─ reads DT_BX_Weapons
├─ reads DT_BX_Ammo
├─ consumes ammo via AC_BX_Inventory
└─ exposes weapon HUD data
```

### 仕様

* 武器スロットは `Primary` / `Secondary` / `Pistol` / `Melee` の 4 種を持つ。
* 各スロットは武器個体 `FBXWeaponInstance` を 0 または 1 個保持できる。
* 武器個体は `WeaponId`(`DT_BX_Weapons` の主キー)と装填弾薬・残弾・耐久・アタッチメントを保持する。
* 弾薬の正本は `AC_BX_Inventory` 側にあり、本コンポーネントは「装填済み弾」の正本のみ持つ。
* 切替は即時切替と長押し収納の双方をサポートする。
* リロードは中断可能とする。
* 射撃モードは `Single` / `Burst` / `Auto` から武器ごとに切替可能。
* HUD には弾数・武器名・射撃モード・耐久を公開する。

### 武器スロット定義

| スロット名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `Primary` | `EBXWeaponSlot` | ✅ | 主武器 |
| `Secondary` | `EBXWeaponSlot` | ✅ | 副武器 |
| `Pistol` | `EBXWeaponSlot` | ✅ | ハンドガン |
| `Melee` | `EBXWeaponSlot` | ✅ | 近接武器 |

### 武器個体構造案

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `WeaponInstanceId` | `FGuid` | ✅ | 武器個体一意 ID |
| `WeaponId` | `FName` | ✅ | `DT_BX_Weapons` 主キー |
| `LoadedAmmoId` | `FName` |  | 装填中弾薬種別 |
| `LoadedAmmoCount` | `int32` | ✅ | 装填済み弾数 |
| `ChamberLoaded` | `bool` |  | 薬室装填中か |
| `DurabilityCurrent` | `float` | ✅ | 現在耐久 |
| `DurabilityMax` | `float` | ✅ | 最大耐久 |
| `CurrentFireMode` | `EBXFireMode` |  | 現在射撃モード |
| `Attachments` | `FGameplayTagContainer` |  | アタッチメント |

### 基本フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `OwnerPlayer` | `APlayerCharacterBase*` | ✅ | 所有プレイヤー |
| `InventoryComponent` | `UAC_BX_Inventory*` | ✅ | 所持品参照 |
| `EquippedWeapons` | `TMap<EBXWeaponSlot, FBXWeaponInstance>` | ✅ | スロット別武器個体 |
| `CurrentSlot` | `EBXWeaponSlot` | ✅ | 現在装備スロット |
| `bIsHolstered` | `bool` | ✅ | 武器収納中か |
| `bIsReloading` | `bool` | ✅ | リロード中か |
| `bIsSwitching` | `bool` | ✅ | 切替中か |
| `bIsAiming` | `bool` | ✅ | ADS 中か |
| `bIsFiring` | `bool` | ✅ | 射撃中か |
| `WeaponLockTags` | `FGameplayTagContainer` |  | 武器ロックタグ |

### リロード関連フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `CurrentReloadType` | `EBXReloadType` |  | Tactical / Empty / Single |
| `ReloadElapsedSec` | `float` |  | 経過時間 |
| `ReloadRemainingSec` | `float` |  | 残り時間 |
| `bReloadInterrupted` | `bool` |  | 中断されたか |

### HUD 公開フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `HudActiveWeaponNameJa` | `FText` |  | 日本語武器名 |
| `HudActiveWeaponNameEn` | `FText` |  | 英語武器名 |
| `HudActiveWeaponAmmoCurrent` | `int32` | ✅ | 現在装填弾数 |
| `HudActiveWeaponAmmoMax` | `int32` | ✅ | 最大弾数 |
| `HudActiveAmmoIdJa` | `FText` |  | 弾薬名(日) |
| `HudActiveAmmoIdEn` | `FText` |  | 弾薬名(英) |
| `HudFireMode` | `EBXFireMode` |  | 射撃モード |
| `HudDurabilityRatio` | `float` |  | 耐久率 |

### C++ 関数シグネチャ

```cpp
UFUNCTION(BlueprintCallable, Category="BX|Weapon")
void InitializeWeaponHandler();

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
bool EquipWeaponToSlot(EBXWeaponSlot Slot, const FBXWeaponInstance& WeaponInstance);

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
bool UnequipWeaponFromSlot(EBXWeaponSlot Slot);

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
bool TrySwitchToSlot(EBXWeaponSlot NewSlot);

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
void HolsterCurrentWeapon();

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
void UnholsterCurrentWeapon();

UFUNCTION(BlueprintPure, Category="BX|Weapon")
bool HasWeaponInSlot(EBXWeaponSlot Slot) const;

UFUNCTION(BlueprintPure, Category="BX|Weapon")
EBXWeaponSlot GetCurrentSlot() const;

UFUNCTION(BlueprintPure, Category="BX|Weapon")
bool GetCurrentWeaponInstance(FBXWeaponInstance& OutInstance) const;

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
bool BeginReload(EBXReloadType ReloadType);

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
void TickReload(float DeltaSeconds);

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
void InterruptReload();

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
void CompleteReload();

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
bool TryFire();

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
void StopFire();

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
void SetAimingState(bool bNewAiming);

UFUNCTION(BlueprintCallable, Category="BX|Weapon")
bool CycleFireMode();

UFUNCTION(BlueprintPure, Category="BX|Weapon")
EBXFireMode GetCurrentFireMode() const;

UFUNCTION(BlueprintCallable, Category="BX|Weapon|Save")
void ExportWeaponHandlerState(FBXWeaponHandlerState& OutState) const;

UFUNCTION(BlueprintCallable, Category="BX|Weapon|Save")
void ImportWeaponHandlerState(const FBXWeaponHandlerState& InState);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Weapon")
void BP_OnWeaponSlotChanged(EBXWeaponSlot NewSlot);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Weapon")
void BP_OnReloadStarted(EBXReloadType ReloadType);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Weapon")
void BP_OnReloadInterrupted();

UFUNCTION(BlueprintImplementableEvent, Category="BX|Weapon")
void BP_OnReloadCompleted();

UFUNCTION(BlueprintImplementableEvent, Category="BX|Weapon")
void BP_OnFire();
```

### Blueprint / C++ 役割分担

* **C++ 側**:
  * 武器個体保持
  * 切替・装填・射撃モード判定
  * 弾薬整合性
  * HUD 公開
  * Save / Load
* **Blueprint 側**:
  * 武器メッシュ表示
  * リロードアニメ
  * 射撃 VFX / SFX
  * ADS カメラ制御
* **アセット配置先**:
  * `Source/BX/Public/Characters/Components/UAC_BX_WeaponHandler.h`
  * `Source/BX/Private/Characters/Components/UAC_BX_WeaponHandler.cpp`
  * `Content/BX/Animations/Weapons/`
  * `Content/BX/VFX/Weapons/`

### 関連章

* §14-4
* §14-5
* §16-4
* §23-4
* §24-1

### 受け入れ条件

* [ ] 4 種類のスロット (`Primary`/`Secondary`/`Pistol`/`Melee`) を保持できる
* [ ] 武器個体 `FBXWeaponInstance` を保持できる
* [ ] スロット切替が即時 / 長押し収納で動作する
* [ ] リロードを開始・中断・完了できる
* [ ] 射撃モードを切替できる
* [ ] HUD に武器名、弾数、射撃モードを表示できる


---

## §23-6 プレイヤー BodyPart / Health コンポーネント

### 概要

本章では、プレイヤーの 7 部位 HP・出血・骨折・痛み・失神・死亡を管理する `AC_BX_HealthBodyParts` を定義する。
C++ クラス名は `UAC_BX_HealthBodyParts`、アタッチ名は `AC_BX_HealthBodyParts` とする。
本作の部位構成は **Head / Chest / Abdomen / LeftArm / RightArm / LeftLeg / RightLeg** の 7 部位とする。

### ASCII クラス / 依存図

```text
UActorComponent
└─ UAC_BX_HealthBodyParts
   └─ owned by APlayerCharacterBase

UAC_BX_HealthBodyParts
├─ updated by damage events
├─ feeds AC_BX_StatusEffects
└─ feeds HUD damage UI
```

### 仕様

* 部位は 7 部位固定。
* 部位ごとに現在 HP / 最大 HP / 出血段階 / 骨折状態 / 痛み段階を持つ。
* 部位 HP が 0 になった場合、部位ごとに以下を判定する。
  * Head 0 → 即死
  * Chest 0 → 即死(または瀕死状態判定後即死)
  * Abdomen 0 → 失神 → 出血加速
  * LeftArm/RightArm 0 → 該当腕の戦闘性能著しく低下
  * LeftLeg/RightLeg 0 → 該当脚の歩行不能
* 各部位 HP は医療品で個別回復できる。
* 出血は時間経過で全体 HP に影響する。
* 骨折は移動・射撃精度に影響する。
* 痛みは画面演出と精度低下に影響する。
* 状態遷移は `AC_BX_StatusEffects` に通知する。

### 部位定義

| 部位名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `Head` | `EBXBodyPart` | ✅ | 頭部 |
| `Chest` | `EBXBodyPart` | ✅ | 胸部 |
| `Abdomen` | `EBXBodyPart` | ✅ | 腹部 |
| `LeftArm` | `EBXBodyPart` | ✅ | 左腕 |
| `RightArm` | `EBXBodyPart` | ✅ | 右腕 |
| `LeftLeg` | `EBXBodyPart` | ✅ | 左脚 |
| `RightLeg` | `EBXBodyPart` | ✅ | 右脚 |

### 部位 HP 構造案

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `BodyPart` | `EBXBodyPart` | ✅ | 部位識別 |
| `CurrentHp` | `float` | ✅ | 現在 HP |
| `MaxHp` | `float` | ✅ | 最大 HP |
| `BleedTier` | `int32` | ✅ | 出血段階(0=なし) |
| `bIsFractured` | `bool` | ✅ | 骨折中か |
| `PainTier` | `int32` | ✅ | 痛み段階 |
| `LastDamageAtUnix` | `int64` |  | 最終被弾時刻 |
| `StateTags` | `FGameplayTagContainer` |  | 状態タグ |

### 基本フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `OwnerPlayer` | `APlayerCharacterBase*` | ✅ | 所有プレイヤー |
| `StatusEffectsComponent` | `UAC_BX_StatusEffects*` | ✅ | 状態異常参照 |
| `BodyPartStates` | `TMap<EBXBodyPart, FBXBodyPartHpState>` | ✅ | 部位状態マップ |
| `bIsAlive` | `bool` | ✅ | 生存中か |
| `bIsUnconscious` | `bool` | ✅ | 失神中か |
| `bIsBleedingActive` | `bool` | ✅ | 出血進行中か |
| `bIsFracturedAny` | `bool` | ✅ | いずれかが骨折中か |
| `OverallPainTier` | `int32` | ✅ | 総合痛み段階 |
| `BleedTickAccumSec` | `float` |  | 出血累積時間 |

### HUD 公開フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `HudHpHead` | `float` | ✅ | 頭部 HP |
| `HudHpChest` | `float` | ✅ | 胸部 HP |
| `HudHpAbdomen` | `float` | ✅ | 腹部 HP |
| `HudHpLeftArm` | `float` | ✅ | 左腕 HP |
| `HudHpRightArm` | `float` | ✅ | 右腕 HP |
| `HudHpLeftLeg` | `float` | ✅ | 左脚 HP |
| `HudHpRightLeg` | `float` | ✅ | 右脚 HP |
| `HudBleedHead` | `int32` | ✅ | 頭部出血 |
| `HudBleedChest` | `int32` | ✅ | 胸部出血 |
| `HudBleedAbdomen` | `int32` | ✅ | 腹部出血 |
| `HudBleedLeftArm` | `int32` | ✅ | 左腕出血 |
| `HudBleedRightArm` | `int32` | ✅ | 右腕出血 |
| `HudBleedLeftLeg` | `int32` | ✅ | 左脚出血 |
| `HudBleedRightLeg` | `int32` | ✅ | 右脚出血 |
| `HudFractureFlags` | `int32` | ✅ | 骨折ビットフラグ |
| `HudPainTier` | `int32` | ✅ | 痛み段階 |

### C++ 関数シグネチャ

```cpp
UFUNCTION(BlueprintCallable, Category="BX|Health")
void InitializeBodyPartsComponent();

UFUNCTION(BlueprintCallable, Category="BX|Health")
void ApplyDamageToBodyPart(EBXBodyPart BodyPart, float DamageAmount, FGameplayTagContainer DamageTags);

UFUNCTION(BlueprintCallable, Category="BX|Health")
void HealBodyPart(EBXBodyPart BodyPart, float HealAmount);

UFUNCTION(BlueprintCallable, Category="BX|Health")
void SetBleedTier(EBXBodyPart BodyPart, int32 NewTier);

UFUNCTION(BlueprintCallable, Category="BX|Health")
void StopBleed(EBXBodyPart BodyPart);

UFUNCTION(BlueprintCallable, Category="BX|Health")
void SetFractureState(EBXBodyPart BodyPart, bool bFractured);

UFUNCTION(BlueprintCallable, Category="BX|Health")
void FixFracture(EBXBodyPart BodyPart);

UFUNCTION(BlueprintCallable, Category="BX|Health")
void SetPainTier(int32 NewPainTier);

UFUNCTION(BlueprintCallable, Category="BX|Health")
void TickBleed(float DeltaSeconds);

UFUNCTION(BlueprintPure, Category="BX|Health")
float GetBodyPartHp(EBXBodyPart BodyPart) const;

UFUNCTION(BlueprintPure, Category="BX|Health")
int32 GetBodyPartBleedTier(EBXBodyPart BodyPart) const;

UFUNCTION(BlueprintPure, Category="BX|Health")
bool IsBodyPartFractured(EBXBodyPart BodyPart) const;

UFUNCTION(BlueprintPure, Category="BX|Health")
bool IsAlive() const;

UFUNCTION(BlueprintPure, Category="BX|Health")
bool IsUnconscious() const;

UFUNCTION(BlueprintCallable, Category="BX|Health|Save")
void ExportHealthState(FBXHealthState& OutState) const;

UFUNCTION(BlueprintCallable, Category="BX|Health|Save")
void ImportHealthState(const FBXHealthState& InState);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Health")
void BP_OnBodyPartDamaged(EBXBodyPart BodyPart, float DamageAmount);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Health")
void BP_OnBodyPartZeroed(EBXBodyPart BodyPart);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Health")
void BP_OnBleedTierChanged(EBXBodyPart BodyPart, int32 NewTier);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Health")
void BP_OnFractureStateChanged(EBXBodyPart BodyPart, bool bFractured);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Health")
void BP_OnDeath();

UFUNCTION(BlueprintImplementableEvent, Category="BX|Health")
void BP_OnUnconscious();
```

### Blueprint / C++ 役割分担

* **C++ 側**:
  * 部位 HP 正本
  * ダメージ適用 / 回復
  * 出血 / 骨折 / 痛み判定
  * 死亡 / 失神判定
  * Save / Load
* **Blueprint 側**:
  * HUD 表示
  * 被弾演出 / 痛み演出
  * 死亡演出
* **アセット配置先**:
  * `Source/BX/Public/Characters/Components/UAC_BX_HealthBodyParts.h`
  * `Source/BX/Private/Characters/Components/UAC_BX_HealthBodyParts.cpp`
  * `Content/BX/UI/HUD/`

### 関連章

* §8-2
* §15-2
* §16-4
* §23-4
* §24-3

### 受け入れ条件

* [ ] 7 部位 (Head/Chest/Abdomen/LeftArm/RightArm/LeftLeg/RightLeg) を保持できる
* [ ] 各部位の HP / 出血 / 骨折 / 痛みを管理できる
* [ ] ダメージ適用で HP が減少する
* [ ] 医療品で部位回復できる
* [ ] 出血は時間経過で HP に影響する
* [ ] 死亡 / 失神判定が動作する
* [ ] HUD に 7 部位の状態を公開できる
* [ ] Save / Load で復元できる


---

## §14-6 防具 DataTable の行構造詳細

### 概要

本章では、`DT_BX_Armors` に格納する防具行構造 `FBXArmorTableRow` を定義する。
本 DataTable は、装備可能な防具の本体性能・ダメージ軽減・耐久・スロット・互換性を保持する正本となる。
`AC_BX_Inventory` から装備指示が出され、`UAC_BX_HealthBodyParts` のダメージ計算で参照される。

### ASCII データ参照図

```text
DT_BX_Armors
└─ FBXArmorTableRow

Used by:
├─ AC_BX_Inventory (equip/unequip)
├─ UAC_BX_HealthBodyParts (damage mitigation)
├─ Merchant UI (sell/buy/repair)
└─ Quest / Loot system
```

### 仕様

* C++ 行構造名は `FBXArmorTableRow` とする。
* DataTable 名は `DT_BX_Armors` とする。
* 主キーは `ArmorId` とする。
* 防具大分類は `ArmorCategory` で表現する (Helmet/BodyArmor/Vest/Plate/Limb)。
* 防具材質は `ArmorMaterialType` で表現する。
* カバー部位は `CoveredBodyParts` で表現する。
* ダメージ軽減は弾種ごとの貫通耐性として保持する。
* 耐久は `DurabilityCurrent / DurabilityMax` で管理し、攻撃を受けるたびに減少する。

### 防具カテゴリ

| カテゴリ名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `Helmet` | `EBXArmorCategory` | ✅ | ヘルメット |
| `BodyArmor` | `EBXArmorCategory` | ✅ | ボディアーマー一体型 |
| `Vest` | `EBXArmorCategory` | ✅ | ベスト |
| `Plate` | `EBXArmorCategory` | ✅ | プレート |
| `LimbArmor` | `EBXArmorCategory` | ✅ | 腕脚プロテクター |

### コアフィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `ArmorId` | `FName` | ✅ | 主キー |
| `DisplayNameJa` | `FText` | ✅ | 日本語表示名 |
| `DisplayNameEn` | `FText` | ✅ | 英語表示名 |
| `DescriptionJa` | `FText` |  | 日本語説明 |
| `DescriptionEn` | `FText` |  | 英語説明 |
| `ArmorCategory` | `EBXArmorCategory` | ✅ | 大分類 |
| `MaterialType` | `EBXArmorMaterialType` | ✅ | 材質 |
| `EquipSlot` | `EBXArmorEquipSlot` | ✅ | 装備スロット |
| `CoveredBodyParts` | `TArray<EBXBodyPart>` | ✅ | カバー部位 |
| `WeightKg` | `float` | ✅ | 重量 |
| `BasePriceCredits` | `int32` | ✅ | 基準価格 |
| `DurabilityMax` | `float` | ✅ | 最大耐久 |
| `ArmorClass` | `int32` | ✅ | 防具クラス段階 |
| `bIsRepairable` | `bool` | ✅ | 修理可能か |
| `bCanBeStored` | `bool` | ✅ | 商人預け可能か |
| `bCanBeSold` | `bool` | ✅ | 売却可能か |
| `LinkedItemId` | `FName` | ✅ | `DT_BX_Items` へのリンク |
| `IconTexture` | `TSoftObjectPtr<UTexture2D>` | ✅ | アイコン |
| `MeshAsset` | `TSoftObjectPtr<USkeletalMesh>` |  | メッシュ |

### ダメージ軽減フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `DamageMitigationRatio` | `float` | ✅ | 基本ダメージ軽減率 |
| `PenetrationResistanceTier` | `int32` | ✅ | 貫通耐性段階 |
| `BluntDamageTransferRatio` | `float` |  | 鈍器ダメージ透過率 |
| `MovementSpeedPenalty` | `float` |  | 移動速度ペナルティ |
| `ErgonomicsPenalty` | `float` |  | 操作性ペナルティ |
| `SoundResistanceLevel` | `int32` |  | 音抑制段階 |

### 関連章

* §14-4
* §14-5
* §16-4
* §23-6
* §24-2

### 受け入れ条件

* [ ] `FBXArmorTableRow` が C++ で定義されている
* [ ] `DT_BX_Armors` が `ArmorId` 主キーで参照できる
* [ ] 5 種類のカテゴリ (Helmet/BodyArmor/Vest/Plate/LimbArmor) を保持できる
* [ ] 7 部位のいずれかをカバーできる
* [ ] 弾種別の貫通耐性段階を持てる
* [ ] 修理 / 売却 / 預け可否を保持できる
* [ ] CSV から DataTable へインポートできる

---

## §8-2 ダメージから状態異常への変換仕様

### 概要

本章では、被弾・転倒・酸素不足・薬物副作用などの **ダメージイベント** が、`UAC_BX_HealthBodyParts` と `UAC_BX_StatusEffects` を通じて、出血・骨折・痛み・脳震盪・失神などの **状態異常** に変換されるルールを定義する。

### ASCII フロー図

```text
DamageEvent (incoming)
   ↓
UAC_BX_HealthBodyParts.ApplyDamageToBodyPart()
   ↓
[Reduce HP] → [Evaluate Bleed/Fracture/Pain Trigger]
   ↓                    ↓
[BleedTier++]    [Fracture=true]    [PainTier++]
   ↓
UAC_BX_StatusEffects.ApplyEffect()
   ↓
[Add to active effects, broadcast HUD]
```

### 仕様

* ダメージは `FBXDamageEvent` 構造体で表現する。
* ダメージ種別は `EBXDamageType` で分類する (Ballistic/Blunt/Explosive/Fall/Poison/Suffocation等)。
* 部位ごとに「出血しきい値」「骨折しきい値」「痛み加算」を定義する。
* 出血段階は 0〜3 の 4 段階。
* 骨折は 0/1 の二値。
* 痛みは 0〜5 の 6 段階(全身合算)。
* 状態異常は `AC_BX_StatusEffects` に通知される。

### ダメージイベント構造体

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `DamageEventId` | `FGuid` | ✅ | イベント ID |
| `TargetBodyPart` | `EBXBodyPart` | ✅ | 被弾部位 |
| `DamageAmount` | `float` | ✅ | ダメージ量 |
| `DamageType` | `EBXDamageType` | ✅ | ダメージ種別 |
| `AmmoIdIfBallistic` | `FName` |  | 弾種(弾薬の場合) |
| `bAfterArmorMitigation` | `bool` | ✅ | 防具軽減後の値か |
| `SourceActor` | `AActor*` |  | 加害者参照 |
| `DamageTags` | `FGameplayTagContainer` |  | 追加タグ |

### 部位別変換ルール表

| 部位 | 出血誘発しきい値 (HP%) | 骨折誘発しきい値 (HP%) | 痛み加算ベース |
| --- | ---: | ---: | ---: |
| `Head` | 30 | (なし) | 2 |
| `Chest` | 35 | 25 | 2 |
| `Abdomen` | 40 | 30 | 2 |
| `LeftArm` | 50 | 40 | 1 |
| `RightArm` | 50 | 40 | 1 |
| `LeftLeg` | 50 | 35 | 1 |
| `RightLeg` | 50 | 35 | 1 |

### 関連章

* §14-5
* §15-2
* §16-4
* §23-6
* §24-3

### 受け入れ条件

* [ ] `FBXDamageEvent` が C++ で定義されている
* [ ] 部位ごとに出血 / 骨折しきい値が定義されている
* [ ] ダメージ適用時に状態異常への変換が判定される
* [ ] 状態異常は `AC_BX_StatusEffects` に通知される
* [ ] 防具軽減後の値で判定される
* [ ] HUD に状態異常が反映される

---

## §16-4 SaveGame 参照構造体詳細

### 概要

本章では、`UBXSaveGame` 内で参照される主要構造体を定義する。
これらの構造体は **プレイヤー進行・部位 HP・状態異常・所持品・武器・依頼・脱出進行・商人状態** を 1 セッション分まとめて保存・復元する正本となる。

### ASCII データ図

```text
UBXSaveGame
├─ FBXPlayerProgress
├─ FBXHealthState
├─ FBXStatusEffectsState
├─ FBXInventoryState
├─ FBXWeaponHandlerState
├─ FBXQuestProgressState
├─ FBXExtractionProgressState
└─ FBXMerchantState[]
```

### 仕様

* 各構造体は **C++ で定義し USTRUCT(BlueprintType) でマーク**する。
* 各構造体に `SaveVersion: int32` を持たせ、互換性破壊時に上げる。
* 構造体は表示文言を保持しない(`FName` ID 参照、UI 側でローカライズ)。
* `FGuid` は個体識別、`FName` はマスタキー。

### FBXPlayerProgress

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `SaveVersion` | `int32` | ✅ | 構造体バージョン |
| `PlayerProfileId` | `FGuid` | ✅ | プロフィール ID |
| `WalletCredits` | `int32` | ✅ | 通貨 |
| `RaidCount` | `int32` | ✅ | 累計レイド数 |
| `LastSavedAtUnix` | `int64` | ✅ | 最終保存時刻 |
| `CurrentRegionId` | `FName` |  | 現在地域 |
| `UnlockedRegions` | `TArray<FName>` |  | 解放地域 |
| `UnlockedNodeIds` | `TArray<FName>` |  | 解放ノード |
| `ProgressTags` | `FGameplayTagContainer` |  | 進行タグ |

### FBXHealthState

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `SaveVersion` | `int32` | ✅ | バージョン |
| `BodyPartStates` | `TArray<FBXBodyPartHpState>` | ✅ | 部位 HP 状態 |
| `bIsAlive` | `bool` | ✅ | 生存中 |
| `bIsUnconscious` | `bool` | ✅ | 失神中 |
| `OverallPainTier` | `int32` | ✅ | 痛み段階 |

### FBXStatusEffectsState

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `SaveVersion` | `int32` | ✅ | バージョン |
| `ActiveEffects` | `TArray<FBXStatusEffectInstance>` | ✅ | 適用中状態異常 |

### FBXInventoryState

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `SaveVersion` | `int32` | ✅ | バージョン |
| `Items` | `TArray<FBXInventoryItemState>` | ✅ | 所持品 |
| `QuickSlots` | `TArray<FGuid>` |  | クイックスロット |
| `TotalWeightKg` | `float` | ✅ | 総重量 |

### FBXWeaponHandlerState

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `SaveVersion` | `int32` | ✅ | バージョン |
| `EquippedWeapons` | `TArray<FBXWeaponInstance>` | ✅ | 装備武器個体 |
| `CurrentSlot` | `EBXWeaponSlot` | ✅ | 現在スロット |

### FBXQuestProgressState

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `SaveVersion` | `int32` | ✅ | バージョン |
| `ActiveQuests` | `TArray<FBXQuestInstance>` | ✅ | 進行中依頼 |
| `CompletedQuestIds` | `TArray<FName>` | ✅ | 完了済 |
| `FailedQuestIds` | `TArray<FName>` |  | 失敗済 |

### FBXExtractionProgressState

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `SaveVersion` | `int32` | ✅ | バージョン |
| `bIsExtractionActive` | `bool` | ✅ | 進行中か |
| `ExtractPointId` | `FName` |  | 脱出ポイント |
| `RemainingExtractSeconds` | `float` |  | 残り時間 |
| `ElapsedExtractSeconds` | `float` |  | 経過時間 |

### FBXMerchantState

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `SaveVersion` | `int32` | ✅ | バージョン |
| `MerchantId` | `FName` | ✅ | 商人 ID |
| `TrustPoints` | `int32` | ✅ | 信頼ポイント |
| `TrustLevel` | `int32` | ✅ | 信頼レベル |
| `StoredItems` | `TArray<FBXInventoryItemState>` |  | 預け在庫 |
| `StoredCredits` | `int32` |  | 預け通貨 |
| `bIsUnlocked` | `bool` | ✅ | 解放済みか |

### 関連章

* §15-1
* §15-2
* §16-1
* §23-4
* §23-5
* §23-6
* §24-1

### 受け入れ条件

* [ ] 全ての参照構造体が C++ で定義されている
* [ ] 各構造体に `SaveVersion` がある
* [ ] 表示文言をセーブせず ID から UI 側で再生成できる


---

## §23-7 プレイヤー Interaction / Loot / Extract 連携

### 概要

本章では、プレイヤーがワールド上の対象へ行う **インタラクト、ルート、脱出開始、依頼関連操作、設備操作** の共通窓口を定義する。
C++ クラス名は `UAC_BX_PlayerInteraction`、アタッチ名は `AC_BX_PlayerInteraction` とする。
本コンポーネントは、`APlayerCharacterBase` の視線先 / 近接対象を評価し、`AC_BX_Inventory`、`UBXQuestSubsystem`、`UBXMerchantNetworkSubsystem`、`ABXExtractPointBase` 等へ橋渡しを行う。

### 仕様

* プレイヤーのインタラクト窓口は `AC_BX_PlayerInteraction` とする。
* 対象検出は **視線中心優先 + 距離判定** を基本とする。
* 同時に複数対象がある場合は以下の優先順位で解決する。
  1. `ABXExtractPointBase`
  2. 依頼関連オブジェクト
  3. ルートコンテナ / 死体
  4. ドア / スイッチ / 端末
  5. その他汎用インタラクト
* 対象ごとに `EBXInteractTargetType` を持つ。
* インタラクト中は移動 / 戦闘を部分ロックできる。
* ルート取得時は `AC_BX_Inventory` に転送要求を送る。
* 依頼関連対象は `UBXQuestSubsystem` へ通知する。
* 商人端末やノード端末は `UBXMerchantNetworkSubsystem` へ通知する。
* 脱出対象は `ABXExtractPointBase` に開始要求を送る。
* 医療使用中、リロード中、武器切替中などの競合状態では対象に応じて拒否または中断処理を行う。

### 対象種別定義

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `ExtractPoint` | `EBXInteractTargetType` | ✅ | 脱出ポイント |
| `LootContainer` | `EBXInteractTargetType` | ✅ | ルートコンテナ |
| `CorpseLoot` | `EBXInteractTargetType` | ✅ | 死体ルート |
| `Door` | `EBXInteractTargetType` | ✅ | ドア |
| `Switch` | `EBXInteractTargetType` | ✅ | 電源 / レバー / スイッチ |
| `Terminal` | `EBXInteractTargetType` | ✅ | 商人 / ノード / 情報端末 |
| `QuestObject` | `EBXInteractTargetType` | ✅ | 依頼対象オブジェクト |
| `Pickup` | `EBXInteractTargetType` | ✅ | 地面アイテム拾得 |
| `Generic` | `EBXInteractTargetType` | ✅ | その他汎用対象 |

### 基本フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `OwnerPlayer` | `APlayerCharacterBase*` | ✅ | 所有プレイヤー |
| `InventoryComponent` | `UAC_BX_Inventory*` | ✅ | 所持品参照 |
| `QuestSubsystem` | `UBXQuestSubsystem*` | ✅ | 依頼 Subsystem |
| `MerchantNetworkSubsystem` | `UBXMerchantNetworkSubsystem*` | ✅ | 商人ネットワーク |
| `FocusedActor` | `TWeakObjectPtr<AActor>` |  | 現在フォーカス対象 |
| `FocusedTargetType` | `EBXInteractTargetType` | ✅ | 現在フォーカス種別 |
| `FocusedDistanceCm` | `float` |  | フォーカス距離 |
| `bHasValidInteractTarget` | `bool` | ✅ | 有効対象あり |
| `bIsInteractionInProgress` | `bool` | ✅ | 進行中か |
| `CurrentInteractionId` | `FGuid` |  | 進行中インタラクト ID |

### C++ 関数シグネチャ

```cpp
UFUNCTION(BlueprintCallable, Category="BX|Interaction")
void InitializeInteractionComponent();

UFUNCTION(BlueprintCallable, Category="BX|Interaction")
void RefreshFocusedTarget();

UFUNCTION(BlueprintPure, Category="BX|Interaction")
bool HasValidFocusedTarget() const;

UFUNCTION(BlueprintCallable, Category="BX|Interaction")
void RequestInteract();

UFUNCTION(BlueprintCallable, Category="BX|Interaction")
void BeginHoldInteraction(float RequiredHoldSec);

UFUNCTION(BlueprintCallable, Category="BX|Interaction")
void TickHoldInteraction(float DeltaSeconds);

UFUNCTION(BlueprintCallable, Category="BX|Interaction")
void CancelCurrentInteraction();

UFUNCTION(BlueprintCallable, Category="BX|Loot")
bool TryOpenLootSource(AActor* LootSourceActor);

UFUNCTION(BlueprintCallable, Category="BX|Loot")
EBXInventoryTransferResult TryTakeItemFromLootSource(FGuid ItemInstanceId, int32 Quantity);

UFUNCTION(BlueprintCallable, Category="BX|Loot")
EBXInventoryTransferResult TryTakeAllFromLootSource();

UFUNCTION(BlueprintCallable, Category="BX|Extract")
bool TryBeginExtractAtFocusedPoint();

UFUNCTION(BlueprintCallable, Category="BX|Quest")
void NotifyQuestObjectInteracted(FName QuestObjectId, FGameplayTag CompletionEventTag);

UFUNCTION(BlueprintCallable, Category="BX|Merchant")
void NotifyMerchantTerminalUsed(FName NodeOrTerminalId);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Interaction")
void BP_OnFocusedTargetChanged(AActor* NewFocusedActor, EBXInteractTargetType NewTargetType);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Interaction")
void BP_OnInteractionStarted(FGuid InteractionId);
```

### 関連章

* §23-4
* §24-1
* §24-2
* §29-5
* §31-1

### 受け入れ条件

* [ ] 視線先と近接候補から有効対象を選べる
* [ ] 脱出ポイント、依頼対象、ルート対象、設備対象を種別判定できる
* [ ] ルート対象から `AC_BX_Inventory` へ転送要求を送れる
* [ ] 脱出ポイントへ開始要求を送れる
* [ ] Quest / MerchantNetwork Subsystem に通知できる
* [ ] 長押しインタラクトを進行 / 中断できる
* [ ] HUD に対象名、操作ヒント、不可理由を表示できる

---

## §24-1 インベントリコンポーネント

### 概要

本章では、プレイヤー所持品の正本を管理する `AC_BX_Inventory` の仕様を定義する。
本コンポーネントはすでに実装済み前提とし、今後の Claude Code 実装・改修時に仕様を固定するための基準として扱う。

### 仕様

* コンポーネント名は `AC_BX_Inventory`、C++ 実装クラスは `UAC_BX_Inventory`。
* `APlayerCharacterBase` に必須アタッチする。
* 正本として保持するのは以下:
  * プレイヤー所持アイテム一覧
  * スタック数量
  * 総重量
  * クイックスロット割当
* 責務外: 商人預け在庫の正本管理、依頼進行の正本管理、部位体力の正本管理、武器スロットの正本管理。
* 表示名は保持せず、`ItemId` から `DT_BX_Items` を参照する。

### 基本フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `OwnerPlayer` | `APlayerCharacterBase*` | ✅ | 所有プレイヤー |
| `InventoryItems` | `TArray<FBXInventoryItemState>` | ✅ | 所持アイテム |
| `CurrentTotalWeightKg` | `float` | ✅ | 現在総重量 |
| `MaxCarryWeightKg` | `float` | ✅ | 最大携行重量 |
| `QuickSlotItemInstanceIds` | `TArray<FGuid>` |  | クイックスロット |
| `WalletCreditsRuntime` | `int32` |  | レイド中通貨 |
| `bInventoryDirty` | `bool` | ✅ | dirty フラグ |

### アイテムカテゴリ保持ルール

| カテゴリ | スタック | 個体管理 | クイックスロット | 備考 |
| --- | --- | --- | --- | --- |
| `Ammo` | 可 | 不要 | 不可 | 弾種ごと |
| `Medical` | 可 | 任意 | 可 | 包帯等 |
| `Consumable` | 可 | 任意 | 可 | 食料 |
| `Utility` | 可 / 不可 | 任意 | 可 | 用途次第 |
| `Quest` | 可 / 不可 | 任意 | 原則不可 | 依頼関連 |
| `KeyItem` | 原則不可 | 可 | 不可 | 鍵 |
| `Weapon` | 不可 | 必須 | 不可 | §23-5 連携 |
| `Armor` | 不可 | 必須 | 不可 | 装備連携 |
| `Attachment` | 可 / 不可 | 可 | 不可 | 個体差 |
| `Junk` | 可 | 任意 | 不可 | 売却用 |

### C++ 関数シグネチャ

```cpp
UFUNCTION(BlueprintCallable, Category="BX|Inventory")
void InitializeInventoryComponent();

UFUNCTION(BlueprintCallable, Category="BX|Inventory")
EBXInventoryTransferResult TryAddItem(
    FName ItemId, int32 Quantity, FGuid RequestedInstanceId,
    bool bAllowCreateNewStack, bool bIgnoreWeightCheck);

UFUNCTION(BlueprintCallable, Category="BX|Inventory")
EBXInventoryTransferResult TryRemoveItemByInstanceId(FGuid ItemInstanceId, int32 Quantity);

UFUNCTION(BlueprintCallable, Category="BX|Inventory")
EBXInventoryTransferResult TryRemoveItemByItemId(FName ItemId, int32 Quantity);

UFUNCTION(BlueprintCallable, Category="BX|Inventory")
void RecalculateTotalWeight();

UFUNCTION(BlueprintPure, Category="BX|Inventory")
bool CanAcceptAdditionalWeight(float AdditionalWeightKg) const;

UFUNCTION(BlueprintPure, Category="BX|Inventory")
EBXCarryState GetCarryState() const;

UFUNCTION(BlueprintCallable, Category="BX|Inventory")
bool AssignItemToQuickSlot(FGuid ItemInstanceId, int32 QuickSlotIndex);

UFUNCTION(BlueprintCallable, Category="BX|Inventory")
EBXInventoryTransferResult TryTransferToMerchantStorage(FName MerchantId, FGuid ItemInstanceId, int32 Quantity);

UFUNCTION(BlueprintCallable, Category="BX|Inventory")
EBXInventoryTransferResult TrySubmitQuestItem(FName QuestId, FGuid ItemInstanceId, int32 Quantity);

UFUNCTION(BlueprintCallable, Category="BX|Inventory")
EBXInventoryTransferResult TryLootFromSource(FName LootSourceId, FGuid ItemInstanceId, int32 Quantity);

UFUNCTION(BlueprintCallable, Category="BX|Inventory")
void ExportInventorySaveState(TArray<FBXInventoryItemState>& OutItems, TArray<FGuid>& OutQuickSlots) const;

UFUNCTION(BlueprintCallable, Category="BX|Inventory")
void ImportInventorySaveState(const TArray<FBXInventoryItemState>& InItems, const TArray<FGuid>& InQuickSlots);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Inventory")
void BP_OnInventoryChanged();
```

### 関連章

* §16-4
* §23-5
* §23-7
* §24-2
* §24-3

### 受け入れ条件

* [ ] `InventoryItems` を正本として保持できる
* [ ] スタック可能アイテムを既存行へ統合できる
* [ ] 非スタック品を個体行として保持できる
* [ ] 総重量を再計算できる
* [ ] クイックスロットへ割当 / 解除できる
* [ ] ルート、商人預け、依頼納品の転送要求を送れる
* [ ] Save / Load で所持品とクイックスロットを復元できる
* [ ] HUD に重量と所持数を表示できる

---

## §24-2 アイテム DataTable の行構造詳細

### 概要

本章では、`DT_BX_Items` に格納するアイテム行構造 `FBXItemTableRow` を定義する。
本 DataTable は、インベントリ、ルート、商人売買、依頼品判定、医療 / 消耗品使用、重量計算、日英表示の正本となる。

### 仕様

* C++ 行構造名は `FBXItemTableRow`、DataTable 名は `DT_BX_Items`、主キーは `ItemId`。
* 表示文字列は日英対応のため、日本語 / 英語を個別に保持する。
* スタック、重量、価格、カテゴリ、使用可否、商人カテゴリ、依頼フラグを必須項目として持つ。
* 武器本体は `DT_BX_Weapons`、防具本体は `DT_BX_Armors` へ接続できるようにする。

### コアフィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `ItemId` | `FName` | ✅ | 主キー |
| `DisplayNameJa` | `FText` | ✅ | 日本語表示名 |
| `DisplayNameEn` | `FText` | ✅ | 英語表示名 |
| `DescriptionJa` | `FText` |  | 日本語説明 |
| `DescriptionEn` | `FText` |  | 英語説明 |
| `ItemCategory` | `EBXItemCategory` | ✅ | 大分類 |
| `Rarity` | `EBXItemRarity` |  | レアリティ |
| `bIsStackable` | `bool` | ✅ | スタック可能か |
| `MaxStackSize` | `int32` | ✅ | 最大スタック数 |
| `WeightPerUnitKg` | `float` | ✅ | 1 個あたり重量 |
| `BasePriceCredits` | `int32` | ✅ | 基準価格 |
| `PriceVarianceRatio` | `float` | ✅ | 価格変動率 |
| `bCanSellToTrader` | `bool` | ✅ | 商人売却可能か |
| `bCanStoreAtMerchant` | `bool` | ✅ | 商人預け可能か |
| `bUsableInRaid` | `bool` | ✅ | レイド中使用可能か |
| `bUsableFromQuickSlot` | `bool` | ✅ | クイックスロット使用可能か |
| `bIsQuestItem` | `bool` | ✅ | 依頼関連か |
| `bIsKeyItem` | `bool` | ✅ | 鍵か |
| `TraderCategoryTag` | `FGameplayTagContainer` | ✅ | 商人表示分類 |

### カテゴリ別追加フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `UseActionType` | `EBXItemUseActionType` |  | 使用アクション種別 |
| `UseDurationSec` | `float` |  | 使用時間 |
| `HealHead` | `float` |  | 頭部回復 |
| `HealChest` | `float` |  | 胸部回復 |
| `HealAbdomen` | `float` |  | 腹部回復 |
| `HealLeftArm` | `float` |  | 左腕回復 |
| `HealRightArm` | `float` |  | 右腕回復 |
| `HealLeftLeg` | `float` |  | 左脚回復 |
| `HealRightLeg` | `float` |  | 右脚回復 |
| `BleedStopPower` | `int32` |  | 止血段階 |
| `FractureFixPower` | `int32` |  | 骨折治療段階 |
| `PainkillerPower` | `int32` |  | 鎮痛段階 |
| `Caliber` | `FName` |  | 弾薬口径 |
| `AmmoCountPerItem` | `int32` |  | 弾数/個 |
| `KeyAccessTag` | `FGameplayTag` |  | 解錠タグ |
| `LinkedWeaponId` | `FName` |  | `DT_BX_Weapons` 参照 |
| `LinkedArmorId` | `FName` |  | `DT_BX_Armors` 参照 |
| `QuestEventTag` | `FGameplayTag` |  | 依頼イベントタグ |

### CSV サンプル

```csv
ItemId,DisplayNameJa,DisplayNameEn,ItemCategory,bIsStackable,MaxStackSize,WeightPerUnitKg,BasePriceCredits,PriceVarianceRatio,bCanSellToTrader,bCanStoreAtMerchant,bUsableInRaid,bIsQuestItem,bIsKeyItem
item_med_bandage,"包帯","Bandage",Medical,true,4,0.2,1200,0.20,true,true,true,false,false
item_food_water_500,"水 500ml","Water 500ml",Consumable,true,2,0.5,900,0.15,true,true,true,false,false
item_keycard_wh_a01,"倉庫A01カード","Warehouse A01 Keycard",KeyItem,false,1,0.1,8000,0.00,false,true,false,false,true
```

### 関連章

* §16-4
* §23-7
* §24-1
* §24-3
* §31-1

### 受け入れ条件

* [ ] `FBXItemTableRow` が C++ で定義されている
* [ ] `DT_BX_Items` が `ItemId` 主キーで参照できる
* [ ] 日英表示名と説明文を保持できる
* [ ] スタック、重量、価格、使用可否を 1 行で定義できる
* [ ] 医療、弾薬、鍵、依頼、武器 / 防具リンク列を持てる
* [ ] CSV から DataTable へインポートできる


---

## §24-3 医療アイテム使用仕様

### 概要

本章では、プレイヤーがレイド中に医療アイテムを使用する仕様を定義する。
本作では `UBXMedicalUseSubsystem` が医療使用の実行ハブとなり、`AC_BX_Inventory`、`AC_BX_StatusEffects`、`AC_BX_HealthBodyParts`、`APlayerCharacterBase` と連携して **止血 / 回復 / 骨折治療 / 鎮痛 / 使用中断** を処理する。

### 仕様

* 医療使用の実行ハブは `UBXMedicalUseSubsystem` とする。
* プレイヤー入力起点は `IA_BX_MedicalUse` とする。
* 使用アイテムの正本は `AC_BX_Inventory` にある。
* 部位 HP 正本は `AC_BX_HealthBodyParts` にある。
* 出血 / 骨折 / 痛み / 脳震盪などの状態異常正本は `AC_BX_StatusEffects` にある。
* 医療用途は最低限以下を持つ:
  * `StopBleed`
  * `HealBodyPart`
  * `FixFracture`
  * `PainRelief`
  * `MultiMedical`
* 使用は即時発動ではなく `UseDurationSec` を伴う。
* 被弾、武器切替、射撃、リロード、スプリント、スライド、強制インタラクトで中断できる。
* 完了時のみ効果適用と消費を行う。
* 対象部位指定型アイテムは `Chest` を含む 7 部位選択に対応する。

### 基本フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `OwnerPlayer` | `APlayerCharacterBase*` | ✅ | 使用主体 |
| `InventoryComponent` | `UAC_BX_Inventory*` | ✅ | 所持品参照 |
| `StatusEffectsComponent` | `UAC_BX_StatusEffects*` | ✅ | 状態異常参照 |
| `HealthBodyPartsComponent` | `UAC_BX_HealthBodyParts*` | ✅ | 部位 HP 参照 |
| `bIsMedicalUseActive` | `bool` | ✅ | 使用中か |
| `CurrentMedicalItemInstanceId` | `FGuid` |  | 使用中アイテム ID |
| `CurrentMedicalTargetBodyPart` | `EBXBodyPart` |  | 対象部位 |
| `MedicalUseElapsedSec` | `float` |  | 経過時間 |
| `MedicalUseRemainingSec` | `float` |  | 残り時間 |
| `LastMedicalInterruptReason` | `EBXMedicalInterruptReason` |  | 最終中断理由 |

### 中断理由一覧

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `TookDamage` | `EBXMedicalInterruptReason` | ✅ | 被弾 |
| `WeaponSwap` | `EBXMedicalInterruptReason` | ✅ | 武器切替 |
| `StartedFire` | `EBXMedicalInterruptReason` | ✅ | 射撃開始 |
| `StartedReload` | `EBXMedicalInterruptReason` | ✅ | リロード開始 |
| `StartedSprint` | `EBXMedicalInterruptReason` | ✅ | スプリント開始 |
| `StartedSlide` | `EBXMedicalInterruptReason` | ✅ | スライド開始 |
| `ForcedInteraction` | `EBXMedicalInterruptReason` | ✅ | 強制インタラクト |

### C++ 関数シグネチャ

```cpp
UFUNCTION(BlueprintCallable, Category="BX|Medical")
void InitializeMedicalUseSubsystem(APlayerCharacterBase* InOwnerPlayer);

UFUNCTION(BlueprintCallable, Category="BX|Medical")
bool RequestUseMedicalItem(FGuid ItemInstanceId, EBXBodyPart TargetBodyPart);

UFUNCTION(BlueprintCallable, Category="BX|Medical")
bool RequestUseMedicalItemByQuickSlot(int32 QuickSlotIndex, EBXBodyPart TargetBodyPart);

UFUNCTION(BlueprintCallable, Category="BX|Medical")
bool BeginMedicalUse(FGuid ItemInstanceId, FName ItemId, EBXBodyPart TargetBodyPart, float UseDurationSec);

UFUNCTION(BlueprintCallable, Category="BX|Medical")
void TickMedicalUse(float DeltaSeconds);

UFUNCTION(BlueprintCallable, Category="BX|Medical")
void InterruptMedicalUse(EBXMedicalInterruptReason InterruptReason);

UFUNCTION(BlueprintCallable, Category="BX|Medical")
void CompleteMedicalUse();

UFUNCTION(BlueprintCallable, Category="BX|Medical")
void ApplyMedicalEffects(const FBXItemTableRow& ItemRow, EBXBodyPart TargetBodyPart);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Medical")
void BP_OnMedicalUseStarted(FName ItemId, EBXBodyPart TargetBodyPart, float DurationSec);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Medical")
void BP_OnMedicalUseInterrupted(EBXMedicalInterruptReason InterruptReason);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Medical")
void BP_OnMedicalUseCompleted(FName ItemId, EBXBodyPart TargetBodyPart);
```

### 関連章

* §23-4
* §23-6
* §24-1
* §24-2
* §29-5

### 受け入れ条件

* [ ] クイックスロットとインベントリの両方から医療使用できる
* [ ] 出血 / 骨折 / 欠損 HP / 痛みに応じて使用可否を判定できる
* [ ] 使用時間を伴う進行型医療ができる
* [ ] 被弾や武器切替で中断できる
* [ ] 完了時のみアイテムを消費する
* [ ] `Chest` を含む 7 部位への回復適用ができる
* [ ] HUD に使用中アイテム名、対象部位、残り時間を表示できる

---

## §31-1 脱出ポイント基底仕様

### 概要

本章では、レイド離脱の中核となる脱出ポイント基底クラスを定義する。
既存実装済み `ABXExtractPointBase` を前提とし、進入判定、可否条件、カウントダウン、失敗 / 成功、UI 連携、セーブ連携を仕様として固定する。

### ASCII クラス階層図

```text
AActor
└─ ABXExtractPointBase
   ├─ BP_BX_ExtractPoint_KansaiIndustrial01_SewerTunnel
   ├─ BP_BX_ExtractPoint_KansaiIndustrial01_BreakwaterGate
   └─ BP_BX_ExtractPoint_*
```

### 仕様

* C++ クラス名は `ABXExtractPointBase`、Blueprint 派生は `BP_BX_ExtractPoint_*` 命名。
* 役割: 進入 / 離脱検出、利用可否判定、必要条件チェック、カウントダウン進行、中断条件処理、成功 / 失敗通知。
* 脱出条件は鍵、電源、任務、時間帯、重量などをタグと数値で表現できるようにする。

### 基本フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `ExtractPointId` | `FName` | ✅ | 脱出ポイント ID |
| `RegionId` | `FName` | ✅ | 所属地域 ID |
| `ExtractType` | `EBXExtractType` | ✅ | Static/Conditional/Timed/KeyRequired/PowerRequired |
| `DisplayNameJa` | `FText` | ✅ | 日本語表示名 |
| `DisplayNameEn` | `FText` | ✅ | 英語表示名 |
| `BaseExtractDurationSec` | `float` | ✅ | 基本脱出時間 |
| `bEnabledByDefault` | `bool` | ✅ | 初期有効か |
| `bCanBeUsedMultipleTimes` | `bool` | ✅ | 複数回利用可能か |
| `bIsExtractionActive` | `bool` | ✅ | 進行中か |
| `RemainingExtractSeconds` | `float` |  | 残り時間 |
| `ElapsedExtractSeconds` | `float` |  | 経過時間 |

### 利用可否条件フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `bRequiresKeyItem` | `bool` | ✅ | 鍵必要か |
| `RequiredKeyItemTag` | `FGameplayTag` |  | 必要キータグ |
| `bRequiresPower` | `bool` | ✅ | 電源必要か |
| `RequiredPowerStateTag` | `FGameplayTag` |  | 必要電源タグ |
| `bRequiresQuestFlag` | `bool` | ✅ | 任務フラグ必要か |
| `RequiredQuestFlag` | `FGameplayTag` |  | 必要任務フラグ |
| `bRequiresTimeWindow` | `bool` | ✅ | 時間帯制限あるか |
| `bRequiresCarryWeightUnderThreshold` | `bool` |  | 重量制限あるか |
| `RequiredMaxCarryWeightKg` | `float` |  | 許可最大総重量 |

### C++ 関数シグネチャ

```cpp
UFUNCTION(BlueprintCallable, Category="BX|Extract")
void InitializeExtractPoint();

UFUNCTION(BlueprintPure, Category="BX|Extract")
bool CanPlayerExtract(APlayerCharacterBase* Player, EBXExtractFailReason& OutFailReason) const;

UFUNCTION(BlueprintCallable, Category="BX|Extract")
bool BeginExtractForPlayer(APlayerCharacterBase* Player);

UFUNCTION(BlueprintCallable, Category="BX|Extract")
void TickExtractProgress(float DeltaSeconds);

UFUNCTION(BlueprintCallable, Category="BX|Extract")
void CompleteExtract();

UFUNCTION(BlueprintCallable, Category="BX|Extract")
void AbortExtract(EBXExtractFailReason FailReason);

UFUNCTION(BlueprintCallable, Category="BX|Extract")
void NotifyPlayerEnteredZone(APlayerCharacterBase* Player);

UFUNCTION(BlueprintCallable, Category="BX|Extract")
void NotifyPlayerExitedZone(APlayerCharacterBase* Player);

UFUNCTION(BlueprintCallable, Category="BX|Extract")
void ExportToExtractionProgressState(FBXExtractionProgressState& OutState) const;

UFUNCTION(BlueprintCallable, Category="BX|Extract")
void ImportFromExtractionProgressState(const FBXExtractionProgressState& InState);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Extract")
void BP_OnExtractStarted(APlayerCharacterBase* Player);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Extract")
void BP_OnExtractAborted(EBXExtractFailReason FailReason);

UFUNCTION(BlueprintImplementableEvent, Category="BX|Extract")
void BP_OnExtractCompleted(APlayerCharacterBase* Player);
```

### 関連章

* §16-4
* §23-7
* §29-5
* §31-2
* §31-3

### 受け入れ条件

* [ ] `ABXExtractPointBase` を C++ 基底として生成できる
* [ ] 脱出ゾーン進入 / 離脱を検知できる
* [ ] 鍵 / 電源 / 任務 / 時間帯 / 重量の条件を評価できる
* [ ] 条件成立時にカウントダウンを開始できる
* [ ] 被弾 / 射撃 / ゾーン離脱 / 条件喪失で中断できる
* [ ] 成功時にレイド結果側へ通知できる
* [ ] `FBXExtractionProgressState` へ Save / Load できる

---

## §29-3 詳細版 商人ハブホーム画面 UI(ChatGPT 詳細化)

### 概要

本章では、プレイヤーが商人ネットワークへ接続した際に最初に表示される **商人ハブのホーム画面 UI** を定義する。
本画面は、現在利用可能な商人、所持通貨、預け在庫の概要、信頼度、アクティブ依頼、地域ノード、重要通知を 1 画面で把握し、各商人個別画面や依頼画面へ遷移するための入口となる。

### 仕様

* Widget Blueprint 名は `WBP_BX_MerchantHub` とする。
* 商人ハブは単一商人画面ではなく、ネットワーク全体の総合トップとする。
* 開く起点: 商人ノード端末使用、セーフルーム内ネットワーク端末使用、特定の復帰直後メニュー。
* 本画面で行うこと:
  * 5商人の状態一覧表示
  * プレイヤー所持通貨表示
  * 商人預け在庫の概要表示
  * 信頼度 / 解放状況表示
  * 進行中依頼の要約表示
  * 現在地域・ノード情報表示
  * 各詳細画面への遷移
* 本画面で行わないこと: 個別アイテム売買の詳細処理、詳細在庫操作、依頼の全文確認、ノード再配置の詳細編集。

### 画面状態フィールド

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `CurrentMerchantHubState` | `EBXMerchantHubState` | ✅ | Home/MerchantDetail/QuestSummary 等 |
| `CurrentFocusedMerchantId` | `FName` |  | 選択中商人 ID |
| `CurrentNetworkNodeId` | `FName` |  | アクセス中ノード ID |
| `bHasUnreadMerchantNotifications` | `bool` | ✅ | 未読通知あり |
| `bHasQuestUpdates` | `bool` | ✅ | 依頼更新あり |
| `bMerchantHubInputLocked` | `bool` | ✅ | 入力ロック中 |

### 上部バー用構造体

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `WalletCredits` | `int32` | ✅ | 所持通貨 |
| `CurrentRegionId` | `FName` | ✅ | 地域 ID |
| `CurrentRegionNameJa` | `FText` | ✅ | 日本語地域名 |
| `CurrentRegionNameEn` | `FText` | ✅ | 英語地域名 |
| `CurrentNodeId` | `FName` | ✅ | ノード ID |
| `UnreadNoticeCount` | `int32` | ✅ | 未読通知数 |
| `ActiveQuestCount` | `int32` | ✅ | 進行中依頼数 |

### C++ 関数シグネチャ

```cpp
UFUNCTION(BlueprintCallable, Category="BX|MerchantUI")
void BuildMerchantHubViewData();

UFUNCTION(BlueprintCallable, Category="BX|MerchantUI")
void RefreshMerchantHubViewData();

UFUNCTION(BlueprintCallable, Category="BX|MerchantUI")
void SetFocusedMerchant(FName MerchantId);

UFUNCTION(BlueprintCallable, Category="BX|MerchantUI")
void RequestOpenMerchantDetail(FName MerchantId);

UFUNCTION(BlueprintCallable, Category="BX|MerchantUI")
void RequestOpenStorageView();

UFUNCTION(BlueprintCallable, Category="BX|MerchantUI")
void RequestOpenQuestSummaryView();

UFUNCTION(BlueprintCallable, Category="BX|MerchantUI")
void RequestCloseMerchantHub();

UFUNCTION(BlueprintImplementableEvent, Category="BX|MerchantUI")
void BP_OnMerchantHubDataRefreshed();

UFUNCTION(BlueprintImplementableEvent, Category="BX|MerchantUI")
void BP_OnFocusedMerchantChanged(FName MerchantId);
```

### 関連章

* §16-3
* §29-4
* §29-5
* §32-1

### 受け入れ条件

* [ ] 5商人の一覧を 1 画面で表示できる
* [ ] 所持通貨、現在地域、現在ノード、通知数を表示できる
* [ ] 左ナビから各サブ画面へ遷移できる
* [ ] 商人カード選択で右サマリーが更新される
* [ ] 日英表示を切り替えられる

---

## §29-4 詳細版 商人個別画面 UI(ChatGPT 詳細化)

### 概要

本章では、商人ハブから遷移する商人個別画面 UI を定義する。
本画面は、各商人の専門機能を利用するための実務画面であり、売買 / 預け / 引き出し / 修理 / 治療 / 運搬 / 情報購入 / 依頼受注の入口となる。

### 仕様

* 画面ルート Widget 名は `WBP_BX_MerchantDetail` とする。
* `WBP_BX_MerchantDetail` は共通ベースとし、商人ごとの差分は表示データとタブの有効 / 無効で制御する。
* 商人ごとの主役割:
  * `merchant_minato`: 雑貨売買、初期預かり
  * `merchant_sawabe`: 治療、医療品売買
  * `merchant_kurogane`: 修理、分解、部品売買
  * `merchant_yoshinaga`: 地域間移送、リスポーン拡張
  * `merchant_amagi`: 情報購入、危険度情報

### 商人別有効タブ表

| 商人ID | 有効タブ |
| --- | --- |
| `merchant_minato` | Trade, Buy, Sell, Storage, Quests, Back |
| `merchant_sawabe` | Medical, Buy, Sell, Storage, Quests, Back |
| `merchant_kurogane` | Repair, Buy, Sell, Storage, Quests, Back |
| `merchant_yoshinaga` | Transport, Storage, Quests, Back |
| `merchant_amagi` | Intel, Buy, Quests, Back |

### C++ 関数シグネチャ

```cpp
UFUNCTION(BlueprintCallable, Category="BX|MerchantDetailUI")
void BuildMerchantDetailViewData(FName MerchantId);

UFUNCTION(BlueprintCallable, Category="BX|MerchantDetailUI")
void RefreshMerchantDetailEntries();

UFUNCTION(BlueprintCallable, Category="BX|MerchantDetailUI")
void SetCurrentMerchantDetailTab(EBXMerchantDetailTab NewTab);

UFUNCTION(BlueprintCallable, Category="BX|MerchantDetailUI")
void SetSelectedEntry(FName EntryId);

UFUNCTION(BlueprintCallable, Category="BX|MerchantDetailUI")
void RequestExecuteSelectedEntry();

UFUNCTION(BlueprintCallable, Category="BX|MerchantDetailUI")
void RequestBackFromMerchantDetail();

UFUNCTION(BlueprintImplementableEvent, Category="BX|MerchantDetailUI")
void BP_OnMerchantDetailDataRefreshed();

UFUNCTION(BlueprintImplementableEvent, Category="BX|MerchantDetailUI")
void BP_OnMerchantDetailTabChanged(EBXMerchantDetailTab NewTab);
```

### 関連章

* §24-2
* §29-3
* §29-5
* §32-1

### 受け入れ条件

* [ ] 5商人で共通ベースの個別画面を表示できる
* [ ] 商人ごとに有効タブが切り替わる
* [ ] 商品 / サービス / 依頼 / 預け在庫の一覧を表示できる
* [ ] 右詳細に価格、重量、条件を表示できる
* [ ] 確認ダイアログを表示できる
* [ ] 日英表示を切り替えられる

---

## §29-5 商人売買フロー仕様

### 概要

本章では、商人個別画面上で実行される購入 / 売却 / 預け / 引き出し / 修理 / 治療 / 移送 / 情報購入 / 依頼納品のフローを定義する。
UI は Blueprint 主導で構成し、価格計算、可否判定、在庫反映、信頼度反映、セーブ反映は C++ 側を正本とする。

### 仕様

* すべての商人操作は `Request → Validate → Confirm → Execute → Commit / Rollback` の順で処理する。
* 価格、信頼度、在庫可否、重量可否は Execute 前に再検証する。
* 成功時のみ反映: 通貨変動、在庫変動、信頼度変動、依頼進行、UI 通知。
* 失敗時はプレイヤー所持品・商人在庫・通貨を変更しない。
* すべての操作結果は `EBXMerchantTransactionResult` を返せるようにする。

### トランザクション共通構造体

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `TransactionId` | `FGuid` | ✅ | トランザクション ID |
| `MerchantId` | `FName` | ✅ | 対象商人 |
| `TransactionType` | `EBXMerchantTransactionType` | ✅ | Buy/Sell/Store/Withdraw/Repair/Heal/Transport/Intel/QuestSubmit |
| `PlayerProfileId` | `FGuid` | ✅ | プロフィール ID |
| `ItemOrEntryId` | `FName` |  | 対象 ID |
| `Quantity` | `int32` |  | 数量 |
| `TotalPriceCredits` | `int32` |  | 最終価格 |
| `TrustDelta` | `int32` |  | 信頼度変動 |
| `Result` | `EBXMerchantTransactionResult` |  | 実行結果 |

### C++ 関数シグネチャ

```cpp
UFUNCTION(BlueprintCallable, Category="BX|MerchantTransaction")
void BeginMerchantTransaction(const FBXMerchantTransactionRequest& Request);

UFUNCTION(BlueprintCallable, Category="BX|MerchantTransaction")
bool ValidateMerchantTransaction(const FBXMerchantTransactionRequest& Request, FBXMerchantTransactionValidationResult& OutResult) const;

UFUNCTION(BlueprintCallable, Category="BX|MerchantTransaction")
void ExecuteMerchantTransaction(const FBXMerchantTransactionRequest& Request);

UFUNCTION(BlueprintCallable, Category="BX|MerchantTransaction")
void CommitMerchantTransaction(const FBXMerchantTransactionResultData& ResultData);

UFUNCTION(BlueprintCallable, Category="BX|MerchantTransaction")
void RollbackMerchantTransaction(const FBXMerchantTransactionRequest& Request);

UFUNCTION(BlueprintCallable, Category="BX|MerchantTransaction")
bool TryPurchaseMerchantOffer(const FBXMerchantPurchaseRequest& Request);

UFUNCTION(BlueprintCallable, Category="BX|MerchantTransaction")
bool TrySellInventoryItem(const FBXMerchantTransferRequest& Request);

UFUNCTION(BlueprintCallable, Category="BX|MerchantTransaction")
bool TryStoreInventoryItem(const FBXMerchantTransferRequest& Request);

UFUNCTION(BlueprintCallable, Category="BX|MerchantTransaction")
bool TryWithdrawStoredItem(const FBXMerchantTransferRequest& Request);

UFUNCTION(BlueprintCallable, Category="BX|MerchantTransaction")
bool TryExecuteMerchantService(const FBXMerchantServiceRequest& Request);

UFUNCTION(BlueprintCallable, Category="BX|MerchantTransaction")
bool TryTurnInMerchantQuest(const FBXMerchantQuestTurnInRequest& Request);

UFUNCTION(BlueprintImplementableEvent, Category="BX|MerchantTransaction")
void BP_OnMerchantTransactionSucceeded(const FBXMerchantTransactionResultData& ResultData);

UFUNCTION(BlueprintImplementableEvent, Category="BX|MerchantTransaction")
void BP_OnMerchantTransactionFailed(EBXMerchantTransactionResult FailureReason);
```

### 関連章

* §24-1
* §24-2
* §24-3
* §29-3
* §29-4

### 受け入れ条件

* [ ] `Request → Validate → Confirm → Execute → Commit / Rollback` フローで処理できる
* [ ] 購入 / 売却 / 預け / 引き出しに対応できる
* [ ] 修理 / 治療 / 移送 / 情報購入に対応できる
* [ ] 依頼納品と報酬適用に対応できる
* [ ] 失敗時に正本を書き換えずロールバックできる
* [ ] 通貨、在庫、信頼度、依頼進行を UI に即時反映できる


---

## §31-2 初期地域 `KansaiIndustrial01` の脱出ポイント一覧

### 概要

本章では、初期地域 **近畿湾岸工業圏 第一区画 (`KansaiIndustrial01`)** に配置する脱出ポイントの一覧と個別仕様を定義する。
本地域は縦切り優先で最初に完成させるため、脱出ポイントも固定脱出 / 条件付き脱出 / 鍵要求脱出 / 電源要求脱出 / 単発脱出を一通り含め、以後の地域展開の雛形とする。

### 仕様

* 初期地域の脱出ポイントは最低 6 箇所とする。
* 構成内訳:
  * 常時有効の基本脱出: 2
  * 条件付き脱出: 2
  * 鍵 / カード要求脱出: 1
  * 電源または任務連動脱出: 1
* すべての脱出ポイントに日英名称を持たせる。
* 一部脱出のみがプレイヤー開始時から利用可能で、残りは探索や条件達成で開放される。

### 地域脱出ポイントマスタ

| `ExtractPointId` | `DisplayNameJa` | `DisplayNameEn` | `ExtractType` | `BaseExtractDurationSec` | `RiskLevel` | `bEnabledByDefault` | `PrimaryConditionTag` |
| --- | --- | --- | --- | ---: | --- | --- | --- |
| `extract_kansai01_sewer_tunnel` | 下水トンネル搬出口 | Sewer Tunnel Exit | `Static` | 7.0 | Low | ✅ |  |
| `extract_kansai01_breakwater_gate` | 防波堤ゲート | Breakwater Gate | `Static` | 8.5 | Medium | ✅ |  |
| `extract_kansai01_warehouse_carddoor` | 倉庫A01裏搬出口 | Warehouse A01 Back Exit | `KeyRequired` | 6.0 | Medium |  | `Access.Warehouse.A01` |
| `extract_kansai01_substation_rail` | 変電所貨物レール | Substation Rail Exit | `PowerRequired` | 10.0 | High |  | `Power.Substation.Online` |
| `extract_kansai01_smuggler_barge` | 密輸バージ接岸点 | Smuggler Barge Dock | `Conditional` | 12.0 | High |  | `Extract.Barge.WindowOpen` |
| `extract_kansai01_overpass_ladder` | 高架補修梯子 | Overpass Maintenance Ladder | `Conditional` | 5.5 | Medium | ✅ | `Extract.Overpass.LightLoadOnly` |

### CSV サンプル

```csv
ExtractPointId,DisplayNameJa,DisplayNameEn,ExtractType,PoiId,RegionId,BaseExtractDurationSec,RiskLevel,bEnabledByDefault,PrimaryConditionTag
extract_kansai01_sewer_tunnel,"下水トンネル搬出口","Sewer Tunnel Exit",Static,poi_sewer_junction_01,KansaiIndustrial01,7.0,Low,true,
extract_kansai01_breakwater_gate,"防波堤ゲート","Breakwater Gate",Static,poi_breakwater_01,KansaiIndustrial01,8.5,Medium,true,
extract_kansai01_warehouse_carddoor,"倉庫A01裏搬出口","Warehouse A01 Back Exit",KeyRequired,poi_warehouse_a01,KansaiIndustrial01,6.0,Medium,false,Access.Warehouse.A01
extract_kansai01_substation_rail,"変電所貨物レール","Substation Rail Exit",PowerRequired,poi_substation_01,KansaiIndustrial01,10.0,High,false,Power.Substation.Online
extract_kansai01_smuggler_barge,"密輸バージ接岸点","Smuggler Barge Dock",Conditional,poi_harbor_barge_01,KansaiIndustrial01,12.0,High,false,Extract.Barge.WindowOpen
extract_kansai01_overpass_ladder,"高架補修梯子","Overpass Maintenance Ladder",Conditional,poi_overpass_01,KansaiIndustrial01,5.5,Medium,true,Extract.Overpass.LightLoadOnly
```

### 関連章

* §31-1
* §31-3
* §31-4
* §32-1

### 受け入れ条件

* [ ] `KansaiIndustrial01` に最低 6 箇所の脱出ポイントが定義されている
* [ ] 固定、条件付き、鍵要求、電源要求の各種別を含む
* [ ] すべての脱出ポイントに日英名称がある
* [ ] 初期有効脱出と後解放脱出を分けて管理できる

---

## §31-3 `KansaiIndustrial01` POI / スポーン / 導線

### 概要

本章では、初期地域 `KansaiIndustrial01` の基礎地理構造を定義する。
対象は、主要 POI、プレイヤースポーン、敵出現の基礎分布、主要導線、危険帯、初期縦切り実装に必要な最小レベル構成である。

### 仕様

* 地域 ID は `KansaiIndustrial01` 固定。
* 「港湾工業エリアの一部区画」を切り出した構成。
* 初期縦切りでは、スポーン、ルート、戦闘、依頼回収、鍵取得、条件付き脱出が 1 地域内で成立することを目的とする。
* 地形要素: 倉庫群、中央ヤード、変電所、下水接続部、防波堤、高架補修通路、密輸バージ接岸区画。

### 主要 POI 一覧

| `PoiId` | `DisplayNameJa` | `DisplayNameEn` | 主用途 | 危険度 |
| --- | --- | --- | --- | --- |
| `poi_warehouse_a01` | 倉庫A01 | Warehouse A01 | 鍵・ルート・依頼 | High |
| `poi_central_yard_01` | 中央資材ヤード | Central Material Yard | 中央交戦地帯 | High |
| `poi_substation_01` | 第一区変電所 | Substation Sector 01 | 電源起動 | High |
| `poi_sewer_junction_01` | 下水接続区画 | Sewer Junction | 安全寄り導線 | Medium |
| `poi_breakwater_01` | 防波堤区画 | Breakwater Sector | 脱出・露出地帯 | Medium |
| `poi_overpass_01` | 高架補修区画 | Overpass Maintenance Zone | 高低差戦闘 | Medium |
| `poi_harbor_barge_01` | 密輸バージ区画 | Smuggler Barge Sector | 高リスク高報酬 | High |
| `poi_pump_room_01` | 排水ポンプ室 | Pump Room | 裏導線・依頼補助 | Medium |

### プレイヤースポーン一覧

| `SpawnPointId` | `DisplayNameJa` | `DisplayNameEn` | 対応側 | 近接 POI |
| --- | --- | --- | --- | --- |
| `spawn_kansai01_west_service_01` | 西側整備通路 | West Service Passage | 西 | `poi_warehouse_a01` |
| `spawn_kansai01_drain_edge_01` | 排水路縁 | Drainage Edge | 南西 | `poi_sewer_junction_01` |
| `spawn_kansai01_breakwater_inlet_01` | 防波堤入口 | Breakwater Inlet | 南 | `poi_breakwater_01` |
| `spawn_kansai01_substation_back_01` | 変電所裏側 | Substation Rear | 北東 | `poi_substation_01` |
| `spawn_kansai01_container_lane_01` | コンテナ通路 | Container Lane | 東 | `poi_harbor_barge_01` |
| `spawn_kansai01_overpass_base_01` | 高架下基部 | Overpass Base | 北 | `poi_overpass_01` |
| `spawn_kansai01_pump_room_entry_01` | ポンプ室入口 | Pump Room Entry | 中央外周 | `poi_pump_room_01` |
| `spawn_kansai01_yard_outer_01` | ヤード外周路 | Yard Outer Route | 中央外周 | `poi_central_yard_01` |

### 基本導線一覧

| ルートID | 名称 | 主経路 | 主な用途 | リスク |
| --- | --- | --- | --- | --- |
| `route_safe_sewer` | 安全寄り導線 | ポンプ室→下水→下水脱出 | 生還重視 | Low |
| `route_mid_yard_push` | 中央突破導線 | 倉庫外縁→中央ヤード→防波堤 | 速攻脱出 | High |
| `route_high_value_barge` | 高価値導線 | コンテナ通路→密輸バージ→沿岸 | 高価値ルート | High |
| `route_condition_substation` | 条件解除導線 | 変電所→貨物レール脱出 | 電源起動利用 | Medium-High |
| `route_light_overpass` | 軽装脱出導線 | 高架下→補修梯子 | 軽装高速脱出 | Medium |

### ASCII 導線図

```text
[West Service]
      |
      v
[Warehouse A01] ---- [Central Yard] ---- [Breakwater Gate]
      |                    |
      |                    |
      v                    v
 [Pump Room] -------- [Sewer Junction]
      |
      |
      v
[Substation] ---- [Overpass] ---- [Container Lane] ---- [Smuggler Barge]
```

### 関連章

* §31-1
* §31-2
* §31-4
* §32-1

### 受け入れ条件

* [ ] 主要 POI が最低 8 箇所定義されている
* [ ] スポーン候補が最低 8 箇所定義されている
* [ ] POI ごとに敵密度の高低が定義されている
* [ ] 安全寄り / 中央突破 / 高価値 / 条件解除導線を持つ
* [ ] 地域データを DataTable から参照できる

---

## §31-4 `KansaiIndustrial01` ルート / 鍵 / 電源 / 依頼配置

### 概要

本章では、初期地域 `KansaiIndustrial01` に配置するインタラクト対象のうち、ルートコンテナ、鍵 / カード、電源設備、依頼対象オブジェクトの配置と役割を定義する。

### 仕様

* 配置カテゴリは 4 種: ルートコンテナ、鍵 / カード、電源設備、依頼オブジェクト。
* すべての配置物は `PoiId` と紐づける。
* 重要オブジェクトは固定候補位置 + 条件付き活性化を基本とする。
* 鍵 / カードは初期地域で最低 1 本の明確な取得導線を持たせる。
* 電源設備は変電所 POI と強く結びつける。

### ルートコンテナ種別定義

| フィールド名 | 型 | 必須 | 説明 |
| --- | --- | --- | --- |
| `ScrapCrate` | `EBXLootContainerType` | ✅ | 雑貨箱 |
| `MedicalCase` | `EBXLootContainerType` | ✅ | 医療箱 |
| `AmmoCase` | `EBXLootContainerType` | ✅ | 弾薬箱 |
| `ToolBox` | `EBXLootContainerType` | ✅ | 工具箱 |
| `Locker` | `EBXLootContainerType` | ✅ | ロッカー |
| `ValuableCase` | `EBXLootContainerType` | ✅ | 高価値ケース |
| `HiddenCache` | `EBXLootContainerType` | ✅ | 隠しキャッシュ |

### ルートコンテナ配置一覧(代表)

| `ObjectPlacementId` | `PoiId` | `ContainerType` | `DisplayNameJa` | `DisplayNameEn` |
| --- | --- | --- | --- | --- |
| `loot_kansai01_wa01_scrap_01` | `poi_warehouse_a01` | `ScrapCrate` | 倉庫雑貨箱A | Warehouse Scrap Crate A |
| `loot_kansai01_wa01_value_01` | `poi_warehouse_a01` | `ValuableCase` | 倉庫高価値ケース | Warehouse Valuable Case |
| `loot_kansai01_sub_med_01` | `poi_substation_01` | `MedicalCase` | 変電所救急箱 | Substation Medical Case |
| `loot_kansai01_sub_tool_01` | `poi_substation_01` | `ToolBox` | 変電所工具箱 | Substation Toolbox |
| `loot_kansai01_sewer_hidden_01` | `poi_sewer_junction_01` | `HiddenCache` | 下水隠しキャッシュ | Sewer Hidden Cache |
| `loot_kansai01_barge_value_01` | `poi_harbor_barge_01` | `ValuableCase` | バージ密輸ケース | Barge Smuggling Case |

### 鍵 / カード一覧

| `ItemId` | `DisplayNameJa` | `DisplayNameEn` | 主用途 |
| --- | --- | --- | --- |
| `item_keycard_wh_a01` | 倉庫A01カード | Warehouse A01 Keycard | 倉庫裏搬出口 / 倉庫進入 |
| `item_key_substation_service` | 変電所保守鍵 | Substation Service Key | 変電所内部アクセス |

### 鍵配置マスタ

| `ObjectPlacementId` | `ItemId` | `PoiId` | `SpawnMode` | `bEnabledByDefault` |
| --- | --- | --- | --- | --- |
| `keyspawn_kansai01_wh_a01_01` | `item_keycard_wh_a01` | `poi_pump_room_01` | `CandidateRandom` | ✅ |
| `keyspawn_kansai01_wh_a01_02` | `item_keycard_wh_a01` | `poi_warehouse_a01` | `HighValueContainer` | ✅ |
| `keyspawn_kansai01_subsvc_01` | `item_key_substation_service` | `poi_substation_01` | `FixedDesk` | ✅ |
| `keyspawn_kansai01_subsvc_02` | `item_key_substation_service` | `poi_central_yard_01` | `EnemyCarrier` | ✅ |

### 電源設備一覧

| `ObjectPlacementId` | `PowerDeviceId` | `PoiId` | `DisplayNameJa` | 主効果 |
| --- | --- | --- | --- | --- |
| `powerobj_kansai01_substation_main_01` | `power_kansai01_substation_main` | `poi_substation_01` | 主変電盤 | レール脱出有効化 |
| `powerobj_kansai01_substation_aux_01` | `power_kansai01_substation_aux` | `poi_substation_01` | 補助切替盤 | 補助照明 / 一部ドア |

### 依頼オブジェクト一覧(代表)

| `ObjectPlacementId` | `QuestObjectId` | `QuestId` | `PoiId` | `DisplayNameJa` | 種別 |
| --- | --- | --- | --- | --- | --- |
| `questobj_kansai01_manifest_01` | `questobj_manifest_a01` | `quest_recover_manifest_01` | `poi_warehouse_a01` | 搬入記録票 | 回収 |
| `questobj_kansai01_logbook_01` | `questobj_substation_logbook` | `quest_substation_logs_01` | `poi_substation_01` | 点検記録簿 | 調査 |
| `questobj_kansai01_transponder_01` | `questobj_barge_transponder` | `quest_barge_signal_01` | `poi_harbor_barge_01` | バージ識別機 | 使用 / 回収 |

### POI 別配置密度表

| `PoiId` | 一般ルート密度 | 高価値密度 | 鍵 / 条件密度 | 依頼密度 |
| --- | ---: | ---: | ---: | ---: |
| `poi_warehouse_a01` | 0.70 | 0.85 | 0.45 | 0.75 |
| `poi_central_yard_01` | 0.55 | 0.35 | 0.20 | 0.30 |
| `poi_substation_01` | 0.50 | 0.40 | 0.80 | 0.55 |
| `poi_sewer_junction_01` | 0.35 | 0.15 | 0.10 | 0.20 |
| `poi_breakwater_01` | 0.20 | 0.10 | 0.05 | 0.10 |
| `poi_overpass_01` | 0.25 | 0.12 | 0.15 | 0.18 |
| `poi_harbor_barge_01` | 0.45 | 0.95 | 0.25 | 0.40 |
| `poi_pump_room_01` | 0.48 | 0.18 | 0.35 | 0.32 |

### 関連章

* §23-7
* §24-2
* §29-5
* §31-2
* §31-3

### 受け入れ条件

* [ ] ルートコンテナ、鍵 / カード、電源、依頼オブジェクトの 4 種を定義できる
* [ ] すべての配置物が `PoiId` と紐づいている
* [ ] 倉庫A01カードなど条件解除用アイテムを配置できる
* [ ] 電源起動で脱出条件タグを有効化できる
* [ ] 依頼対象オブジェクトが Quest 進行イベントを送れる
* [ ] POI 別配置密度をデータで参照できる



---

# §22 ドローンシステム

> 以下の §22-1 〜 §22-5 は ChatGPT v3 仕様書から統合した新規章。
> 統合日: 2026-04-28
> プレイヤー運用前提のドローンシステム(地上 2 種 + 空中 2 種 = 計 4 種類)。

## §22-1 偵察ドローン種別追加（プレイヤー運用前提）

### 概要
BX に、プレイヤーがレイド中に運用できる小型偵察ドローンを追加する。  
本章では **地上ドローン 2 種** と **空中ドローン 2 種** の合計 **4種類** を定義し、主用途を **偵察・索敵・ルート確認・危険確認** とする。  
敵勢力による運用は完全禁止ではないが、**演出用または限定イベント用に留め、基本運用者は主人公のみ** とする。

### 仕様
- 本章で追加するドローンは、直接戦闘火力ではなく **偵察・視認・索敵補助** を主目的とする。
- ドローンの基本運用者は **プレイヤー** とする。
- 敵勢力による使用は以下に限定する。
  - 特定ストーリーイベント
  - 一部の高難度固定ロケーション演出
  - ボス級エリアの限定ギミック
- 通常の雑魚敵、巡回敵、一般勢力は基本的にドローンを常用しない。
- 実装対象のドローンは以下の **4種類** とする。
  - `Drone.Ground.Light`
  - `Drone.Ground.Heavy`
  - `Drone.Air.Light`
  - `Drone.Air.Heavy`
- 軽量型は **スピード・取り回し・偵察向き** とする。
- 重量型は **安定性・稼働時間・被弾耐性・索敵持続** を重視する。
- 地上ドローンは段差・障害物・床面ルートを利用する。
- 空中ドローンは天井高、通路幅、障害物密度の影響を受ける。
- ドローンは武器ではなく、装備スロット上は **特殊ガジェット** 扱いとする。
- ドローン展開中、プレイヤー本体は無防備になりやすいため、完全な安全装置にはしない。
- ドローン視点中は以下のいずれかで運用可能とする。
  - プレイヤーその場停止 + ドローン遠隔視点
  - プレイヤー簡易自動しゃがみ待機 + ドローン遠隔視点
- プレイヤーはドローンを回収または破棄できる。
- ドローン破壊時は、そのレイド中は再使用不可、または予備機がある場合のみ再展開可とする。
- 将来的に静音型、回収型、妨害型へ拡張する余地は残すが、本章では **軽量 / 重量の偵察型 4種類** のみを定義する。

### データ構造

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `DroneId` | `FName` | ✅ | 主キー。例: `drone_ground_light_01` |
| `DisplayName` | `FText` | ✅ | 表示名 |
| `DroneType` | `ENUM_BX_DroneType` | ✅ | ドローン種別（Ground / Air） |
| `WeightClass` | `ENUM_BX_DroneWeightClass` | ✅ | 重量区分（Light / Heavy） |
| `BaseMoveSpeed` | `float` | ✅ | 基本移動速度（cm/s） |
| `MaxOperationRadius` | `float` | ✅ | 行動半径（cm） |
| `VisionRange` | `float` | ✅ | 索敵距離（cm） |
| `VisionAngleDeg` | `float` | ✅ | 索敵視野角（度） |
| `TurnRateDegPerSec` | `float` | ✅ | 旋回速度（度/秒） |
| `BatteryLifeSec` | `float` | ✅ | 連続稼働時間（秒） |
| `DeployTimeSec` | `float` | ✅ | 展開時間（秒） |
| `RecallTimeSec` | `float` | ✅ | 回収時間（秒） |
| `HPMax` | `float` | ✅ | 最大耐久 |
| `NoiseLevel` | `float` | ✅ | 動作音レベル係数 |
| `DetectionPriority` | `int32` | ✅ | 敵 AI に見つかりやすさの優先度 |
| `MeshRef` | `TSoftObjectPtr<UStaticMesh>` |  | 表示メッシュ |
| `MoveSoundRef` | `TSoftObjectPtr<USoundBase>` |  | 移動音 |
| `DestroyFXRef` | `TSoftObjectPtr<UNiagaraSystem>` |  | 破壊 FX |
| `BehaviorTreeRef` | `TSoftObjectPtr<UBehaviorTree>` |  | 自律移動補助 BT |
| `BlackboardRef` | `TSoftObjectPtr<UBlackboardData>` |  | Blackboard |
| `bEnemyCanUse` | `bool` | ✅ | 敵が使用可能か |
| `EnemyUseMode` | `ENUM_BX_DroneEnemyUseMode` | ✅ | None / RareEventOnly / BossAreaOnly |
| `bIsStoryLocked` | `bool` | ✅ | ストーリー解放制御 |
| `RequiredStoryFlag` | `FName` |  | 必要ストーリーフラグ |

### CSV サンプル(該当する場合)

```csv
DroneId,DisplayName,DroneType,WeightClass,BaseMoveSpeed,MaxOperationRadius,VisionRange,VisionAngleDeg,TurnRateDegPerSec,BatteryLifeSec,DeployTimeSec,RecallTimeSec,HPMax,NoiseLevel,DetectionPriority,MeshRef,MoveSoundRef,DestroyFXRef,BehaviorTreeRef,BlackboardRef,bEnemyCanUse,EnemyUseMode,bIsStoryLocked,RequiredStoryFlag
drone_ground_light_01,"Ground Recon Drone Light",Ground,Light,620,1800,1400,95,260,90,1.6,1.3,28,0.55,2,"/Game/BX/World/Drones/Meshes/SM_BX_Drone_Ground_Light.SM_BX_Drone_Ground_Light","/Game/BX/Audio/Drones/SFX_BX_Drone_Ground_Move.SFX_BX_Drone_Ground_Move","/Game/BX/FX/Drones/NS_BX_Drone_Destroy.NS_BX_Drone_Destroy","/Game/BX/AI/Drones/BT_BX_Drone_Ground_Light.BT_BX_Drone_Ground_Light","/Game/BX/AI/Drones/BB_BX_Drone_Common.BB_BX_Drone_Common",true,RareEventOnly,false,
drone_ground_heavy_01,"Ground Recon Drone Heavy",Ground,Heavy,470,2400,1550,100,180,150,2.2,1.9,46,0.68,2,"/Game/BX/World/Drones/Meshes/SM_BX_Drone_Ground_Heavy.SM_BX_Drone_Ground_Heavy","/Game/BX/Audio/Drones/SFX_BX_Drone_Ground_Heavy_Move.SFX_BX_Drone_Ground_Heavy_Move","/Game/BX/FX/Drones/NS_BX_Drone_Destroy.NS_BX_Drone_Destroy","/Game/BX/AI/Drones/BT_BX_Drone_Ground_Heavy.BT_BX_Drone_Ground_Heavy","/Game/BX/AI/Drones/BB_BX_Drone_Common.BB_BX_Drone_Common",true,BossAreaOnly,false,
drone_air_light_01,"Air Recon Drone Light",Air,Light,760,2200,1650,110,320,75,1.8,1.5,24,0.62,3,"/Game/BX/World/Drones/Meshes/SM_BX_Drone_Air_Light.SM_BX_Drone_Air_Light","/Game/BX/Audio/Drones/SFX_BX_Drone_Air_Move.SFX_BX_Drone_Air_Move","/Game/BX/FX/Drones/NS_BX_Drone_Destroy.NS_BX_Drone_Destroy","/Game/BX/AI/Drones/BT_BX_Drone_Air_Light.BT_BX_Drone_Air_Light","/Game/BX/AI/Drones/BB_BX_Drone_Common.BB_BX_Drone_Common",true,BossAreaOnly,false,
drone_air_heavy_01,"Air Recon Drone Heavy",Air,Heavy,590,2800,1900,120,210,135,2.4,2.0,40,0.74,3,"/Game/BX/World/Drones/Meshes/SM_BX_Drone_Air_Heavy.SM_BX_Drone_Air_Heavy","/Game/BX/Audio/Drones/SFX_BX_Drone_Air_Heavy_Move.SFX_BX_Drone_Air_Heavy_Move","/Game/BX/FX/Drones/NS_BX_Drone_Destroy.NS_BX_Drone_Destroy","/Game/BX/AI/Drones/BT_BX_Drone_Air_Heavy.BT_BX_Drone_Air_Heavy","/Game/BX/AI/Drones/BB_BX_Drone_Common.BB_BX_Drone_Common",true,BossAreaOnly,false,
```

### Blueprint / C++ 役割分担

- **C++ 側**:
  - `FBXDroneTableRow` 定義
  - ドローン基底 Actor `ABXDroneBase`
  - 展開、回収、バッテリー消費、視点切替、偵察結果共有ロジック
  - 敵限定使用フラグとイベント制御
- **Blueprint 側**:
  - 地上 / 空中 / 軽量 / 重量の見た目差分
  - カメラ演出、ライト点灯、回転演出
  - エリアごとの巡回ポイント、イベント時の敵利用設定
- **アセット配置先**:
  - C++: `Source/BX/Public/Data/Drones/FBXDroneTableRow.h`
  - C++: `Source/BX/Public/AI/Drones/ABXDroneBase.h`
  - C++: `Source/BX/Private/AI/Drones/ABXDroneBase.cpp`
  - DataTable: `Content/BX/Data/Common/Drones/DT_BX_Drones`
  - Blueprint: `Content/BX/AI/Drones/`
  - Mesh: `Content/BX/World/Drones/Meshes/`
  - Audio: `Content/BX/Audio/Drones/`
  - CSV ソース: `DataSource/Drones/DT_BX_Drones.csv`

### UI

```text
┌──────────────────────────────────────┐
│ DRONE LINK ACTIVE                    │
│ Battery: ███████░░░ 72%              │
│ Range: 12m / 18m                     │
│ Mode: Recon                          │
└──────────────────────────────────────┘
```

### 関連章
- §14-5
- §15-1
- §15-2
- §18-2
- §18-3
- §21-2

### 受け入れ条件
- [ ] `DT_BX_Drones` に 4 種類のドローン行が存在する
- [ ] プレイヤーが 4 種類を展開・回収できる
- [ ] 軽量型と重量型で速度 / 稼働時間 / 耐久の差がある
- [ ] ドローン破壊時にその機体が使用不能になる
- [ ] 敵のドローン使用が RareEventOnly / BossAreaOnly に限定できる

---

## §22-2 地上ドローン（軽量型）

### 概要
地上ドローン軽量型は、床面・配管・通路・狭所を高速移動して周辺状況を確認する軽量偵察ユニットである。  
小回り、段差越え、狭所侵入に優れるが、長距離持続索敵や高所対応は弱い。

### 仕様
- 種別は `Ground`、重量区分は `Light` とする。
- コンセプトは **軽量型 / スピード重視 / 行動半径はやや狭い**。
- プレイヤーの先行偵察、近距離ルート確認、曲がり角確認に向く。
- 旋回性能は高く、狭い場所でも切り返ししやすい。
- 小段差の跳び越え能力を持つ。
- `JumpHeightCm` を持ち、低い障害物を超えられる。
- ただし高低差に弱く、広い吹き抜けや階段連続区間では効率が落ちる。
- 行動半径は狭めで、プレイヤー本体から遠く離れにくい。
- 地上音・転がり音があり、敵に気づかれる余地を残す。
- 火力は持たず、偵察専用に徹する。
- 倉庫、団地、排水路、研究施設下層との相性が良い。

### データ構造

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `JumpHeightCm` | `float` | ✅ | 越えられる段差高さ（cm） |
| `MaxSlopeDeg` | `float` | ✅ | 登坂可能最大角度 |
| `CorneringBoostRatio` | `float` | ✅ | 曲がりやすさ補正 |
| `FloorTraceDistanceCm` | `float` | ✅ | 床追従判定距離 |
| `PreferredPathTag` | `FGameplayTag` |  | 優先経路タグ |
| `bCanUseVentRoute` | `bool` | ✅ | ダクト / 狭所経路使用可否 |

### CSV サンプル(該当する場合)

```csv
DroneId,JumpHeightCm,MaxSlopeDeg,CorneringBoostRatio,FloorTraceDistanceCm,PreferredPathTag,bCanUseVentRoute
drone_ground_light_01,55,38,1.30,24,"Path.Ground.Tight",true
```

### Blueprint / C++ 役割分担

- **C++ 側**:
  - 地上ドローン移動モード
  - 段差判定、床追従、簡易ジャンプ処理
  - 近距離偵察ルート制御
- **Blueprint 側**:
  - ジャンプ演出
  - ホイール回転、ライト点滅、狭所侵入アニメ表現
  - ルートポイント配置
- **アセット配置先**:
  - C++: `Source/BX/Public/AI/Drones/ABXDroneGroundBase.h`
  - Blueprint: `Content/BX/AI/Drones/Ground/BP_BX_Drone_Ground_Light`
  - BT: `Content/BX/AI/Drones/BT_BX_Drone_Ground_Light`
  - BB: `Content/BX/AI/Drones/BB_BX_Drone_Common`

### UI

```text
┌──────────────────────────────────────┐
│ GROUND DRONE LIGHT ACTIVE            │
│ Jump Ready / Tight Route Scan        │
└──────────────────────────────────────┘
```

### 関連章
- §22-1
- §18-3

### 受け入れ条件
- [ ] 地上軽量ドローンが小段差を越えられる
- [ ] 狭所で高い旋回性能を示す
- [ ] 行動半径が地上重量型より狭い
- [ ] 倉庫や団地で自然に移動できる
- [ ] プレイヤーの先行偵察用途として扱える

---

## §22-3 地上ドローン（重量型）

### 概要
地上ドローン重量型は、軽量型より遅いが、稼働時間と安定性に優れた中距離偵察ユニットである。  
床面偵察を長く続けられ、多少の被弾にも耐えるが、小回りとジャンプ性能は低い。

### 仕様
- 種別は `Ground`、重量区分は `Heavy` とする。
- コンセプトは **重量型 / 安定性重視 / 稼働時間長め**。
- 軽量型より移動速度は低い。
- 行動半径は軽量型より広い。
- 耐久は地上軽量型より高い。
- 小段差は越えられるが、ジャンプ性能は低い。
- 旋回性能は軽量型より低い。
- 長めの連続偵察、保守的な通路確認、重装エリアの探索に向く。
- 騒音は軽量型より大きい。
- 下層施設、工場通路、長い保守路との相性が良い。

### データ構造

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `JumpHeightCm` | `float` | ✅ | 越えられる段差高さ（cm） |
| `MaxSlopeDeg` | `float` | ✅ | 登坂可能最大角度 |
| `CorneringBoostRatio` | `float` | ✅ | 曲がりやすさ補正 |
| `FloorTraceDistanceCm` | `float` | ✅ | 床追従判定距離 |
| `StabilityRatio` | `float` | ✅ | 操作安定度補正 |
| `bCanPushSmallDebris` | `bool` | ✅ | 小型障害物を押し退け可能か |

### CSV サンプル(該当する場合)

```csv
DroneId,JumpHeightCm,MaxSlopeDeg,CorneringBoostRatio,FloorTraceDistanceCm,StabilityRatio,bCanPushSmallDebris
drone_ground_heavy_01,35,42,0.88,28,1.35,true
```

### Blueprint / C++ 役割分担

- **C++ 側**:
  - 地上重量型移動モード
  - 安定度補正、慣性強め制御
  - 連続偵察時のバッテリー効率補正
- **Blueprint 側**:
  - 重量感のある移動演出
  - 接地エフェクト、重めの走行音
  - 長通路向けルートポイント配置
- **アセット配置先**:
  - C++: `Source/BX/Public/AI/Drones/ABXDroneGroundBase.h`
  - Blueprint: `Content/BX/AI/Drones/Ground/BP_BX_Drone_Ground_Heavy`
  - BT: `Content/BX/AI/Drones/BT_BX_Drone_Ground_Heavy`
  - BB: `Content/BX/AI/Drones/BB_BX_Drone_Common`

### UI

```text
┌──────────────────────────────────────┐
│ GROUND DRONE HEAVY ACTIVE            │
│ Stable Scan / Long Route Recon       │
└──────────────────────────────────────┘
```

### 関連章
- §22-1
- §18-3

### 受け入れ条件
- [ ] 地上重量型が地上軽量型より遅い
- [ ] 地上重量型が地上軽量型より高耐久である
- [ ] 地上重量型が地上軽量型より長時間稼働する
- [ ] 旋回性能が軽量型より低い
- [ ] 長い保守路や工場通路で偵察用途として扱える

---

## §22-4 空中ドローン（軽量型）

### 概要
空中ドローン軽量型は、上空から高速で索敵し、プレイヤーに周辺視界を与える軽量偵察ユニットである。  
高速・高旋回・広視野に優れるが、閉所や障害物密度の高い場所では挙動制限を受ける。

### 仕様
- 種別は `Air`、重量区分は `Light` とする。
- コンセプトは **軽量型 / スピード重視 / 行動半径はやや狭いが視野は広い**。
- 旋回性能は非常に高い。
- 低空ホバリングと短距離ダッシュを使い分ける。
- 地上ドローンよりジャンプ概念は不要だが、上下移動範囲を持つ。
- `HoverHeightMinCm`〜`HoverHeightMaxCm` の範囲で移動する。
- 視野角、索敵距離、再捕捉性能は地上型より高い。
- ただし閉所・天井低い場所・障害物密度の高い場所では速度が落ちる。
- 風切り音やプロペラ音があるため、近距離なら敵に気づかれる余地を残す。
- 空中型は屋外、吹き抜け、研究施設上層、広い工業区画との相性が良い。
- 偵察能力は高いが、持続時間は空中重量型より短い。

### データ構造

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `HoverHeightMinCm` | `float` | ✅ | 最低ホバー高度（cm） |
| `HoverHeightMaxCm` | `float` | ✅ | 最大ホバー高度（cm） |
| `VerticalMoveSpeed` | `float` | ✅ | 上下移動速度（cm/s） |
| `CorneringBoostRatio` | `float` | ✅ | 曲がりやすさ補正 |
| `ObstacleAvoidanceRadiusCm` | `float` | ✅ | 障害物回避半径 |
| `bCanUseOutdoorPatrol` | `bool` | ✅ | 屋外巡回可否 |

### CSV サンプル(該当する場合)

```csv
DroneId,HoverHeightMinCm,HoverHeightMaxCm,VerticalMoveSpeed,CorneringBoostRatio,ObstacleAvoidanceRadiusCm,bCanUseOutdoorPatrol
drone_air_light_01,120,320,340,1.45,85,true
```

### Blueprint / C++ 役割分担

- **C++ 側**:
  - 空中軽量型移動モード
  - ホバー高度制御
  - 上下移動と障害物回避
  - 広視野偵察ロジック
- **Blueprint 側**:
  - プロペラ回転、ライト点滅、ホバー演出
  - 高度差の見た目表現
  - 屋外 / 屋内の巡回ポイント設定
- **アセット配置先**:
  - C++: `Source/BX/Public/AI/Drones/ABXDroneAirBase.h`
  - Blueprint: `Content/BX/AI/Drones/Air/BP_BX_Drone_Air_Light`
  - BT: `Content/BX/AI/Drones/BT_BX_Drone_Air_Light`
  - BB: `Content/BX/AI/Drones/BB_BX_Drone_Common`

### UI

```text
┌──────────────────────────────────────┐
│ AIR DRONE LIGHT ACTIVE               │
│ Wide Scan / Hover Recon              │
└──────────────────────────────────────┘
```

### 関連章
- §22-1
- §18-3

### 受け入れ条件
- [ ] 空中軽量型がホバー高度範囲内で移動する
- [ ] 地上型より高い旋回性能を示す
- [ ] 視野角と索敵距離が地上型より広い
- [ ] 屋外または吹き抜け区画で自然に移動できる
- [ ] 高速・短時間の偵察用途として扱える

---

## §22-5 空中ドローン（重量型）

### 概要
空中ドローン重量型は、軽量型より低速だが、広めの行動半径、長い稼働時間、安定した上空監視能力を持つ重量偵察ユニットである。  
上空観測や中距離の継続監視に向くが、取り回しは軽量型より鈍い。

### 仕様
- 種別は `Air`、重量区分は `Heavy` とする。
- コンセプトは **重量型 / 継続監視重視 / 稼働時間長め**。
- 軽量型より移動速度は低い。
- 行動半径は軽量型より広い。
- 耐久は空中軽量型より高い。
- ホバー安定性が高く、長時間観測向き。
- 旋回性能は軽量型より低い。
- プロペラ音は軽量型より大きい。
- 広い工業地帯、東京外縁、吹き抜け施設、港湾上空などとの相性が良い。
- 狭い屋内では軽量型より扱いづらい。

### データ構造

| フィールド名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `HoverHeightMinCm` | `float` | ✅ | 最低ホバー高度（cm） |
| `HoverHeightMaxCm` | `float` | ✅ | 最大ホバー高度（cm） |
| `VerticalMoveSpeed` | `float` | ✅ | 上下移動速度（cm/s） |
| `CorneringBoostRatio` | `float` | ✅ | 曲がりやすさ補正 |
| `ObstacleAvoidanceRadiusCm` | `float` | ✅ | 障害物回避半径 |
| `StabilityRatio` | `float` | ✅ | ホバー安定度補正 |

### CSV サンプル(該当する場合)

```csv
DroneId,HoverHeightMinCm,HoverHeightMaxCm,VerticalMoveSpeed,CorneringBoostRatio,ObstacleAvoidanceRadiusCm,StabilityRatio
drone_air_heavy_01,150,420,260,1.05,110,1.40
```

### Blueprint / C++ 役割分担

- **C++ 側**:
  - 空中重量型移動モード
  - 長時間観測時の安定補正
  - 慣性強めの空中制御
- **Blueprint 側**:
  - 重量感あるホバー演出
  - プロペラ音、姿勢安定演出
  - 広域巡回ポイント設定
- **アセット配置先**:
  - C++: `Source/BX/Public/AI/Drones/ABXDroneAirBase.h`
  - Blueprint: `Content/BX/AI/Drones/Air/BP_BX_Drone_Air_Heavy`
  - BT: `Content/BX/AI/Drones/BT_BX_Drone_Air_Heavy`
  - BB: `Content/BX/AI/Drones/BB_BX_Drone_Common`

### UI

```text
┌──────────────────────────────────────┐
│ AIR DRONE HEAVY ACTIVE               │
│ Stable Hover / Long Watch            │
└──────────────────────────────────────┘
```

### 関連章
- §22-1
- §18-3

### 受け入れ条件
- [ ] 空中重量型が空中軽量型より遅い
- [ ] 空中重量型が空中軽量型より高耐久である
- [ ] 空中重量型が空中軽量型より長時間稼働する
- [ ] 旋回性能が軽量型より低い
- [ ] 広域監視用途として扱える
