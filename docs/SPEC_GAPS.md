# 実装前に埋めるべき仕様ギャップ

現 SPEC.md は **方針と構造は十分**だが、C++ / DataTable に落とし込む際に具体値が不足する箇所がいくつかある。ここに列挙しておき、ChatGPT と詰めるときの TODO リストにする。

> 優先度: ★★★ = Phase 1 実装前に必須 / ★★ = Phase 2〜3 前に必要 / ★ = あとでよい

---

## G-01 ★★★ セーブ構造の具体的フィールド

**現状**: §16-1 でブロック名(`ProfileMeta`, `MerchantNetworkState` 等)は列挙されているが、各ブロック内のフィールド型が未定義。

**必要な追加**:

### FBXProfileMeta

| フィールド | 型 | 説明 |
|---|---|---|
| `SaveVersion` | `int32` | マイグレーション用 |
| `ProfileId` | `FGuid` | |
| `ProfileName` | `FString` | |
| `GameMode` | `EBXGameMode` | HardcoreReal / HardcoreMerchant |
| `CreatedAt` | `FDateTime` | |
| `LastPlayedAt` | `FDateTime` | |
| `TotalRaids` | `int32` | |
| `TotalDeaths` | `int32` | |

### FBXMerchantNetworkState

| フィールド | 型 | 説明 |
|---|---|---|
| `MerchantStates` | `TMap<FName, FBXMerchantState>` | MerchantId → 個別状態 |
| `UnlockedRespawnNodes` | `TArray<FName>` | 解放済ノード ID |

### FBXMerchantState(商人1人分)

| フィールド | 型 | 説明 |
|---|---|---|
| `TrustLevel` | `int32` | 1〜4 |
| `TrustPoints` | `int32` | 次段階までの進捗 |
| `StoredInventory` | `TArray<FBXInventoryItem>` | |
| `ActiveQuests` | `TArray<FName>` | |
| `CompletedQuests` | `TArray<FName>` | |

### FBXPlayerProgress, FBXRegionState, FBXRunSnapshot

同様に要定義。

---

## G-02 ★★★ 武器 DataTable のフィールド定義

**現状**: §14-1 でカテゴリ(HG/SMG/AR/SG/DMR/SR)、§30-5 で価格帯はあるが、DT_BX_Weapons の行構造が未定。

**必要な追加**: `FBXWeaponTableRow` の全フィールド

```
WeaponId (FName, PK)
DisplayName (FText)
Category (EBXWeaponCategory)
Caliber (FName)              — "9mm" / "5.56" / "7.62" / "12G" / ".308"
FireModes (TArray<EBXFireMode>) — SemiAuto / FullAuto / Burst / Pump / Bolt
BaseDamage (float)
MuzzleVelocity (float)       — 発射速度(m/s)。弾道計算用
RPM (int32)
VerticalRecoil (float)
HorizontalRecoil (float)
Ergonomics (int32)            — タルコフ系の扱いやすさ
DefaultMagSize (int32)
ReloadTimeSec (float)
TacticalReloadTimeSec (float) — 弾残しリロード
ADSTimeSec (float)
Weight (float)               — kg
DurabilityMax (float)
BasePriceCredits (int32)
AllowedAttachments (FGameplayTagContainer)
SkeletalMesh (TSoftObjectPtr<USkeletalMesh>)
FireSound (TSoftObjectPtr<USoundBase>)
MuzzleFlashFX (TSoftObjectPtr<UNiagaraSystem>)
```

同様に: 弾薬(`FBXAmmoTableRow`)、防具(`FBXArmorTableRow`)、医療(`FBXMedicalTableRow`)も要定義。

---

## G-03 ★★★ ダメージモデル(弾 × 防具の交互作用)

**現状**: §14-3 で「9mmは装甲に弱め」「5.56は汎用」等の方針はあるが、**計算式と数値表が未定**。

**必要な追加**:
- 防具クラス(Armor Class)の段階定義(例: Class1〜Class6)
- 弾ごとの **貫通値(Penetration)** と **衝撃値(Damage)** の具体数値
- 貫通成否の判定式(`Penetration >= ArmorClass * K` 等)
- 貫通時の減衰係数
- 部位別ダメージ倍率の具体値(§9-3 の部位に対して)

例:
```
FBXDamageCalc:
  - PenetrationValue (float)    : 弾側
  - ArmorClass (int32)           : 防具側
  - ShotDamage (float)
  - PartMultiplier (TMap<EBXBodyPart, float>) — Head=3.0, Chest=1.5, Limbs=0.7
  - Formula: if(Pen >= Class*40) { Full damage } else { Blunt damage * 0.3 }
```

---

## G-04 ★★ AI Blackboard キー一覧

**現状**: §13-3 で Blackboard キー定義済みとあるが、**キー名の列挙がない**。Claude Code で BT Task / Service を書くには必要。

**必要な追加**:

```
BB_BX_AI_Common のキー候補:
  TargetActor (Object)
  LastKnownTargetLocation (Vector)
  LastTargetSeenTimeSec (Float)
  HasLineOfSight (Bool)
  CoverLocation (Vector)
  SelfState (Enum: Idle/Patrol/Suspicious/Combat/Search/Flee/Reload)
  HealthPct (Float)
  Ammo (Int)
  IsReloading (Bool)
  SquadId (Name)
  IsSquadLeader (Bool)
  AlertLevel (Float 0.0-1.0)
  FlankTargetLocation (Vector)
```

---

## G-05 ★★ 依頼(Quest)データ構造

**現状**: §17-2 で依頼カテゴリは列挙、§17-3 で依頼チェーン例はあるが、DT_BX_Quests の行構造が未定。

**必要な追加**:

```
FBXQuestTableRow:
  QuestId (FName, PK)
  MerchantId (FName)         — 発行元商人
  QuestType (EBXQuestType)   — 納品/回収/調査/排除/搬送/解放/特殊
  TitleText (FText)
  DescriptionText (FText)
  Prerequisites (TArray<FName>) — 前提クエスト
  RequiredItems (TArray<FBXItemRequirement>)
  RequiredKillTarget (FName)    — 排除依頼時
  RequiredRegion (FName)        — 調査依頼時
  TimeLimit (float)             — 0 = 無期限
  RewardCredits (int32)
  RewardTrustPoints (int32)
  RewardItems (TArray<FBXItemGrant>)
  UnlocksRespawnNode (FName)    — 解放依頼時
  UnlocksRegion (FName)
```

---

## G-06 ★★ リスポーンノード定義

**現状**: §11-2 でノード種別(隠れ家/民家/診療所/倉庫/地下避難室)、§11-3 で解放条件は列挙済み。DT 化の型が未定。

**必要な追加**:

```
FBXRespawnNodeRow:
  NodeId (FName, PK)
  DisplayName (FText)
  Region (FName)
  NodeType (EBXRespawnNodeType)
  OwnerMerchantId (FName)        — 商人が匿う場合
  RequiredTrustLevel (int32)     — 解放条件(Trust)
  RequiredQuestId (FName)        — 解放条件(依頼)
  RequiredKeyItem (FName)        — 解放条件(特殊鍵)
  RequiredRegionStability (float)— 解放条件(地域安全度)
  UseCostCredits (int32)         — 利用コスト
  RespawnDebuffProfile (FName)   — デバフ強度プロファイル
  SpawnTransform (FTransform)    — 再開位置
  IsTemporarilyClosed (bool)
```

---

## G-07 ★ 経済初期値の CSV 化準備

**現状**: §30-5〜§30-8 に価格帯が書いてあるが、範囲表記(例: 4,000〜8,000)なので、CSV に落とす際に「最小値・最大値」または「代表値」のどちらを格納するか方針が未定。

**推奨**: 代表値(範囲の中央)+ 変動レンジ の2列で持つと、商人・地域差の変動計算が楽。

```csv
WeaponId,BasePriceCredits,PriceVarianceRatio
pistol_9mm_01,6000,0.3
```

---

## G-08 ★ レイド時間・昼夜サイクル

**現状**: §20 で「レイド時間長さ」「天候 / 昼夜の影響詳細」が TBD と明記されている。

**推奨決定事項**:
- 1レイド最大時間(タルコフは 40分前後)
- ゲーム内時間の進行速度(実時間 1分 = ゲーム内 X分)
- 脱出確定までの待機時間(脱出ゲージ)

この値は DT 化せず、`DA_BX_RaidConfig_<Region>` に持つのが扱いやすい。

---

## G-09 ★ GameplayTag 体系

**現状**: 言及なし。UE5 で AI・状態異常・装備条件・依頼条件を綺麗に管理するなら GameplayTag を導入推奨。

**推奨**: `Config/Tags/BXGameplayTags.ini` を用意して以下の階層を定義:
- `Status.Bleed.Normal` / `Status.Bleed.Heavy`
- `Status.Fracture` / `Status.Pain` / `Status.Dehydration` / `Status.Hunger`
- `Damage.Bullet.9mm` / `Damage.Bullet.556` / `Damage.Blunt` / `Damage.Explosive`
- `AI.State.Patrol` / `AI.State.Combat` 等
- `Region.Kansai.Industrial01.Port` 等

---

## 次のアクション

上記のうち **★★★(G-01〜G-03)は Phase 1 実装前に必須**。ChatGPT と次セッションで SPEC.md に追記してから C++ 実装に入るのが安全。

- 追記場所の提案:
  - G-01 → §16 に `§16-3 セーブ構造体フィールド詳細` を追加
  - G-02 → §14 に `§14-4 武器データテーブル行定義` を追加
  - G-03 → §14 に `§14-5 ダメージ計算モデル` を追加
  - G-04 → §13 に `§13-4 Blackboard キー一覧` を追加
  - G-05 → §17 に `§17-4 依頼データテーブル行定義` を追加
  - G-06 → §11 に `§11-6 リスポーンノード行定義` を追加
