# CLAUDE.md

> **このファイルは Claude Code がプロジェクト起動時に自動で読み込みます。**
> 以下の規約を全作業の前提として遵守してください。

---

## 0. Claude Code への最重要指示(必ず最初に読む)

### 0-1. 作業プロセスの原則

1. **仕様書を最優先で読む**: 実装依頼を受けたら、まず `docs/SPEC.md` の該当章(§X-Y)を `view` で読むこと。仕様書に無い機能を勝手に実装しない。
2. **章番号での参照を徹底**: ユーザーが「§14-4 を実装して」と言ったら、**必ず SPEC.md の §14-4 を読んでから**作業開始。
3. **不明点は推測せず質問**: 仕様書に書かれていない項目、曖昧な記述があったら、勝手に決めず 2〜4 択でユーザーに質問する。
4. **大きな変更は Plan を先に出す**: 5ファイル以上に影響する変更、新規クラス 3 つ以上の追加は、先に実装計画を提示して承認を得る。
5. **回答は日本語**。

### 0-2. あなた(Claude Code)の担当範囲

| できる | できない |
|---|---|
| `Source/BX/**/*.{h,cpp}` の作成・編集 | `.uasset` / `.umap` の直接編集 |
| `BX.Build.cs` の依存モジュール追加 | Blueprint グラフの作成・編集 |
| `Config/*.ini` の編集 | Widget のビジュアルレイアウト編集 |
| `DataSource/*.csv` の生成・編集 | Material ノードの編集 |
| `docs/*.md` の更新 | Animation / Level / VFX の編集 |
| `.uproject` / `.gitattributes` の編集 | UE5 エディタ操作 |

**Blueprint 側の作業が必要なときは、ユーザーが UE5 エディタで実行できる「手順書」を Markdown で出力する**こと。Claude Code は BP を直接作れないので、C++ 基底クラスを用意した上で、派生 BP の作成手順をテキスト化して渡す。

### 0-3. ユーザーとの役割分担

- **ユーザー**: ChatGPT と共に `docs/SPEC.md` を執筆する。UE5 エディタで Blueprint・Material・Level を作業する。ビルド・テストを実行する。
- **ChatGPT**: `docs/SPEC.md` に仕様章を追記する(あなたはこの作業に関与しない)。
- **あなた(Claude Code)**: SPEC.md を読み、C++ / CSV / Config を生成する。必要なら BP 作成手順書を出力する。

### 0-4. 必ず避ける失敗パターン

- ❌ SPEC.md を読まずに実装を始める
- ❌ 命名規則を勝手に変える(§4 の接頭辞表を無視した新しいプレフィックスを作る等)
- ❌ フォルダ構成を無視した場所にファイルを置く
- ❌ C++ 追加時に `BX.Build.cs` の依存モジュール更新を忘れる
- ❌ `UPROPERTY` にカテゴリや `BlueprintReadWrite` を付け忘れて BP から使えなくする
- ❌ 「テストコード書きました」と言うだけで実行しない(可能ならビルドコマンドまで案内する)

---

## 1. プロジェクト概要

- **コードネーム**: **BLACKOUT EXFIL**(略称 **BX**)
- **エンジン**: Unreal Engine **5.4**(固定。5.5 以降には絶対に上げない)
- **ジャンル**: オフライン専用 高難易度 脱出サバイバル FPS
- **参考方向性**: BO6 風の操作感 + タルコフ以上の損失・緊張
- **ワールド**: 日本本土準拠・多層密度方式(まず近畿湾岸工業圏 第一区画を縦切り)
- **ターゲット**: Windows (Steam)
- **開発体制**: 個人開発(TK001_DEV-MAIN)

### 現時点の方針ハイライト(詳細は SPEC.md)

- **固定拠点なし・商人ネットワーク型**(SPEC §5, §10)
- **2モード構成**: リアルハードコア / 商人預け保持ハードコア(SPEC §4)
- **縦切り優先**: 近畿湾岸工業圏 第一区画 → 拡張(SPEC §23)
- **既存 5 商人**: 古物商ミナト / 闇医者サワベ / 解体屋クロガネ / 運び屋ヨシナガ / 情報屋アマギ

### 関連ドキュメント

| ファイル | 担当 | 内容 |
|---|---|---|
| `CLAUDE.md` | — | **このファイル**。実装規約(どう実装するか) |
| `docs/SPEC.md` | ChatGPT 執筆 | 仕様書(何を実装するか)。章番号(§X-Y)で参照 |
| `docs/SPEC_GAPS.md` | 参照用 | 実装前に埋めるべきギャップ(G-01〜G-09) |
| `docs/CONVENTIONS.md` | 参照用 | 命名規則・コード規約の補足 |
| `docs/WORKFLOW.md` | 参照用 | 日々の作業フロー |

---

## 2. 技術方針

### 2-1. Blueprint と C++ の役割分担

Blueprint 中心 + 必要に応じて C++ の段階的ハイブリッド方式。

| 判断基準 | Blueprint | C++ |
|---|---|---|
| プロトタイピング・試行錯誤 | ◎ | △ |
| 計算量が多い処理(AI, 経路探索, 大量ループ) | × | ◎ |
| 基底クラス・インターフェース | △ | ◎ |
| Data Table 行構造体(`FTableRowBase` 派生) | × | ◎(**必須**) |
| `USaveGame` 派生 | △ | ◎ |
| `UGameInstanceSubsystem` 派生 | × | ◎ |
| ゲームプレイの細かなパラメータ調整 | ◎ | × |
| UI ロジック(Widget 中身) | ◎ | △(必要時のみ基底) |

**原則**: **C++ で「土台(基底クラス・データ構造・重い処理)」→ Blueprint で「派生・挙動・調整」**。

### 2-2. C++ → BP 公開の必須事項

C++ を書くときは、BP 側から使えることを常に意識する:

```cpp
// 正しい例
UCLASS(BlueprintType, Blueprintable)
class BX_API APlayerCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacterBase();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Stats",
              meta=(ClampMin="0.0", ClampMax="200.0"))
    float MaxHealth = 100.0f;

    UFUNCTION(BlueprintCallable, Category="BX|Combat")
    virtual void ApplyBleeding(float Intensity);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UCameraComponent> FirstPersonCamera;
};
```

**必須**:
- `UCLASS` に `BlueprintType, Blueprintable` を適切に付ける
- BP 公開プロパティには `UPROPERTY(EditAnywhere/BlueprintReadWrite)` と `Category="BX|..."` を必ず付与
- BP から呼ぶ関数には `UFUNCTION(BlueprintCallable)` を付ける
- ポインタは `TObjectPtr<T>`(GC 安全)、遅延ロードは `TSoftObjectPtr<T>`
- カテゴリ名は `BX|Stats` のように `BX|` プレフィックスで階層化

---

## 3. ディレクトリ構造(絶対厳守)

```
<ProjectRoot>/
├── BX.uproject
├── CLAUDE.md
├── README.md
├── .gitignore
├── .gitattributes
├── Config/
│   ├── DefaultEngine.ini
│   ├── DefaultGame.ini
│   ├── DefaultInput.ini
│   └── Tags/
│       └── BXGameplayTags.ini
├── Content/
│   └── BX/
│       ├── Core/           ← GI, GM, GS, PC, SaveGame, 共通 Enum/Struct の BP
│       ├── Blueprints/     ← 汎用 BP(ドア、コンテナ、エレベータ、発電機)
│       ├── Characters/     ← プレイヤー、敵AI、ボス、商人、一般人NPC
│       ├── Weapons/        ← 武器 BP、マガジン、アタッチメント
│       ├── Items/          ← 医療品、消耗品、資材、鍵、特殊
│       ├── AI/             ← BT, BB, EQS, AI用 DA, 巡回パス
│       ├── UI/             ← HUD, メニュー, 商人ハブ, インベントリ
│       ├── Audio/, FX/, Materials/, Meshes/, Animations/
│       ├── Maps/
│       │   ├── Regions/
│       │   │   └── KansaiIndustrial01/
│       │   │       ├── Levels/      ← LV_BX_KansaiIndustrial01_P 等
│       │   │       ├── Blockout/, Lighting/, SetDress/, Splines/, Landmarks/
│       │   └── Test/                ← LV_Test_Combat 等
│       ├── Data/
│       │   ├── Common/              ← DT_BX_Weapons 等の広域マスタ
│       │   └── Regions/
│       │       └── KansaiIndustrial01/
│       │           ├── DT_Loot_KansaiIndustrial01
│       │           ├── DT_AI_KansaiIndustrial01
│       │           └── DT_Extract_KansaiIndustrial01
│       ├── Systems/        ← Subsystem 系 BP
│       ├── World/          ← World Partition, Data Layer, PCG, HLOD
│       └── Dev/            ← 開発・検証用(本番ビルド除外)
├── Source/
│   └── BX/                 ← Content/BX/ をミラーした C++ 階層
│       ├── BX.Build.cs
│       ├── BX.h / BX.cpp   ← モジュールエントリ
│       ├── Public/
│       │   ├── Core/       ← AGameMode_BX, UGameInstance_BX, USaveGame_BX 等
│       │   ├── Characters/ ← APlayerCharacterBase, AEnemyBase
│       │   ├── Weapons/    ← AWeaponBase, UWeaponComponent
│       │   ├── Items/      ← UItemBase, UInventoryComponent
│       │   ├── AI/         ← AEnemyAIControllerBase, BTTask_*, BTService_*
│       │   ├── UI/         ← UHUD_BX, UUserWidget_BX 基底
│       │   ├── Data/       ← FBXItemRow, FBXWeaponTableRow 等の行構造体
│       │   ├── Save/       ← USaveGame_BX, FBXProfileMeta 等の構造体
│       │   └── Systems/    ← UMerchantNetworkSubsystem 等
│       └── Private/        ← Public と同構造で .cpp 配置
├── docs/
│   ├── SPEC.md             ← メイン仕様書
│   ├── SPEC_GAPS.md        ← 実装前に埋めるギャップリスト
│   ├── CONVENTIONS.md      ← 命名・コード規約補足
│   └── WORKFLOW.md
└── DataSource/             ← DT インポート前 CSV/JSON
    ├── weapons.csv
    ├── items.csv
    ├── ammo.csv
    ├── armor.csv
    ├── medical.csv
    ├── economy.csv
    └── regions/
        └── kansai_industrial_01/
            ├── loot.csv
            ├── ai_spawn.csv
            └── extracts.csv
```

**新規ファイル作成時の配置ルール**:
- `.h` / `.cpp` → 機能カテゴリに対応する `Source/BX/Public/<Cat>/` と `Source/BX/Private/<Cat>/` のペア
- CSV → `DataSource/` 直下(共通)または `DataSource/regions/<region>/`(地域固有)
- 配置先に迷ったら、新規フォルダを勝手に作らず**ユーザーに確認**する

---

## 4. 命名規則(絶対厳守)

### 4-1. アセット接頭辞(勝手に増やさない)

| 接頭辞 | 用途 | 例 |
|---|---|---|
| `BP_` | 一般 Blueprint | `BP_BX_LootContainer_Warehouse01` |
| `CH_` | Character | `CH_BX_Player`, `CH_BX_AI_Looter` |
| `PC_` | PlayerController | `PC_BX_Raid` |
| `GM_` | GameMode | `GM_BX_Raid`, `GM_BX_MerchantHub` |
| `GS_` | GameState | `GS_BX_Raid` |
| `GI_` | GameInstance | `GI_BX` |
| `SG_` | SaveGame | `SG_BX_Profile` |
| `AC_` | ActorComponent | `AC_BX_Inventory` |
| `WBP_` | Widget Blueprint | `WBP_BX_MerchantHub` |
| `DT_` | DataTable | `DT_BX_Weapons` |
| `DA_` | DataAsset | `DA_BX_Merchant_Minato` |
| `BT_` | Behavior Tree | `BT_BX_AI_CommonCombat` |
| `BB_` | Blackboard | `BB_BX_AI_Common` |
| `EQS_` | EQS | `EQS_BX_FindFlank` |
| `M_` | Material | `M_BX_Concrete` |
| `MI_` | Material Instance | `MI_BX_Concrete_Wet` |
| `SM_` | Static Mesh | `SM_BX_ContainerA` |
| `SK_` | Skeletal Mesh | `SK_BX_Player` |
| `T_` | Texture | `T_BX_UI_Icon_9mm` |
| `SFX_` | Sound | `SFX_BX_Gunshot_556` |
| `NS_` | Niagara System | `NS_BX_MuzzleFlash_AR` |
| `AN_` | Animation Sequence | `AN_BX_Reload_AR` |
| `AM_` | AnimMontage | `AM_BX_Reload_AR_1P` |
| `BS_` | BlendSpace | `BS_BX_Locomotion_1P` |
| `AO_` | AimOffset | `AO_BX_Player_1P` |
| `PP_` | Post Process | `PP_BX_LowHealth` |
| `LV_` | Level | `LV_BX_KansaiIndustrial01_P` |
| `DL_` | Data Layer | `DL_BX_KansaiIndustrial01_Port` |
| `PCG_` | PCG Graph | `PCG_BX_Suburb_LowDensity` |
| `CUR_` | Curve | `CUR_BX_RecoilAR` |
| `STR_` | Struct(BP用) | `STR_BX_InventoryItem` |
| `ENUM_` | Enum(BP用) | `ENUM_BX_DamageType` |

### 4-2. C++ クラス・型命名

- UE5 標準プレフィックス(`A`/`U`/`F`/`E`/`I`)+ 末尾 `_BX`(クラスのみ)
  - `AGameModeRaid_BX`, `UGameInstance_BX`, `USaveGame_BX`
  - `APlayerCharacterBase`(基底は `Base` サフィックスで `_BX` 省略可)
- 構造体は `FBX` プレフィックス(末尾 `_BX` ではない):`FBXItemRow`, `FBXWeaponTableRow`, `FBXInventoryItem`
- Enum は `EBX` プレフィックス:`EBXWeaponCategory`, `EBXDamageType`, `EBXGameMode`
- Interface は `IBX` と `UBX...Interface` のペア:`IBXInteractable`, `UBXInteractableInterface`

### 4-3. C++ ヘッダの書き方

```cpp
// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
// Forward declarations
class UCameraComponent;
class USpringArmComponent;

#include "PlayerCharacterBase.generated.h"  // ← 必ず最後

UCLASS(BlueprintType, Blueprintable, Abstract)
class BX_API APlayerCharacterBase : public ACharacter
{
    GENERATED_BODY()
    // ...
};
```

**Include 順**(必ず守る):
1. 対応ヘッダ(`.cpp` の最初の include)
2. `CoreMinimal.h` と UE5 フレームワーク
3. Forward declaration
4. 自プロジェクト内ヘッダ
5. `*.generated.h`(**必ず最後**)

### 4-4. 命名時の禁止事項

- 地域名・クラス名・アセット名に漢字/カナを使わない(`KansaiIndustrial01` OK / `近畿湾岸01` NG)
- 接頭辞表にないプレフィックスを勝手に作らない
- `Temp`, `Test`, `Old`, `Copy` を本番フォルダに残さない(`/BX/Dev/` か `/BX/Maps/Test/` へ)
- 同じ用途で BP と DT が同名にならないよう、用途を含める

---

## 5. データ設計

### 5-1. Data Table vs Data Asset

| 用途 | 選択 | 例 |
|---|---|---|
| 行が多い、CSV で一括編集したい | **DataTable** | 武器・弾薬・防具・医療・依頼・リスポーンノード |
| 1件ずつがリッチ、エディタで個別編集したい | **DataAsset** | 商人の個性・ボス設定・地域イベントセット・アタッチメント構成 |

### 5-2. DataTable 行構造体の標準パターン

```cpp
// Source/BX/Public/Data/WeaponTableRow.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "BX/Public/Data/BXEnums.h"
#include "WeaponTableRow.generated.h"

USTRUCT(BlueprintType)
struct BX_API FBXWeaponTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon")
    EBXWeaponCategory Category = EBXWeaponCategory::AR;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Weapon")
    FName Caliber;

    // ... 各フィールドには Category 必須

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Assets")
    TSoftObjectPtr<USoundBase> FireSound;
};
```

### 5-3. CSV → DataTable 連携ルール

1. CSV は `DataSource/` 配下に UTF-8 (BOM なし) で保存
2. CSV の1列目は常に `Name`(DT の Row Name 列)
3. 列名は C++ 構造体のフィールド名と**完全一致**
4. Enum 値はシリアライズ名(`EBXWeaponCategory::AR` → CSV では `AR`)
5. TSoftObjectPtr は `/Game/...` 形式のパス文字列

ユーザーに Data Table を作ってもらう手順書を出すときは以下フォーマットで:

```
## DT 作成手順: DT_BX_Weapons

1. UE5 エディタで /Content/BX/Data/Common/ を開く
2. 右クリック → Miscellaneous → Data Table
3. Pick Row Structure で `FBXWeaponTableRow` を選択 → OK
4. 名前を `DT_BX_Weapons` にして保存
5. DT を開き、右上の Reimport アイコン → DataSource/weapons.csv を指定
6. Save All
```

---

## 6. SaveGame 設計

### 6-1. 基本方針

- **家ベースではなく商人ネットワークベース保存**(SPEC §16)
- `USaveGame_BX` 1 クラス + 6 つの `USTRUCT` ブロック構成
- 各構造体に `int32 SaveVersion` 必須(マイグレーション用)
- 通常はバイナリセーブ、デバッグ時のみ JSON ダンプ(`FJsonObjectConverter::UStructToJsonObjectString`)

### 6-2. 構造ブロック一覧

1. `FBXProfileMeta` - SaveVersion, ProfileId, GameMode, 日時, 累計
2. `FBXGlobalSettings` - 音量, 感度, キーコンフィグ
3. `FBXMerchantNetworkState` - 商人別 Trust / 在庫 / 解放ノード
4. `FBXPlayerProgress` - 通貨, 依頼進行, 発見地域
5. `FBXRegionState` - 地域フラグ, イベント状態
6. `FBXRunSnapshot` - レイド中断セーブ(条件付き)

詳細なフィールド定義は **SPEC §16-3** を参照(未記載の場合は SPEC_GAPS.md §G-01 を参照)。

### 6-3. 2モード差分

| モード | 死亡時の挙動 |
|---|---|
| リアルハードコア | 進行資産全消去(設定・実績のみ保持) |
| 商人預け保持ハードコア | 出撃中持ち物のみロスト、預け在庫・Trust は保持 |

---

## 7. Enhanced Input 前提

- すべての入力は **Enhanced Input** で実装
- `Content/BX/Core/Input/` に以下を配置:
  - `IMC_BX_Default`(通常操作)
  - `IMC_BX_MerchantHub`(商人ハブ内)
  - `IMC_BX_UI`(メニュー内)
- `IA_*`(Input Action)は同フォルダ
- 将来のキーコンフィグ画面のため `DA_BX_InputBindings` の枠だけは用意しておく

---

## 8. GameplayTag 体系

プロジェクト独自タグは `Config/Tags/BXGameplayTags.ini` に定義:

```
Status.Bleed.Normal / Status.Bleed.Heavy
Status.Fracture / Status.Pain / Status.Dehydration / Status.Hunger
Damage.Bullet.9mm / Damage.Bullet.556 / Damage.Blunt / Damage.Explosive
AI.State.Patrol / AI.State.Combat / AI.State.Search
Region.Kansai.Industrial01.Port(地域階層)
```

新しいタグ階層を作る前に、既存階層に収まらないか必ず確認する。

---

## 9. Build.cs の依存モジュール管理

`Source/BX/BX.Build.cs` を編集するとき、機能カテゴリごとに以下のモジュールが必要:

| 機能 | 追加するモジュール |
|---|---|
| UI (Widget) | `"UMG"`, `"Slate"`, `"SlateCore"` |
| Enhanced Input | `"EnhancedInput"` |
| AI / BT | `"AIModule"`, `"GameplayTasks"`, `"NavigationSystem"` |
| GameplayTag | `"GameplayTags"` |
| Niagara VFX | `"Niagara"` |
| MetaSound | `"MetaSoundEngine"` |
| JSON 保存 | `"Json"`, `"JsonUtilities"` |
| World Partition | `"WorldPartition"` |
| PCG | `"PCG"` |

**C++ コードを追加するたびに必要モジュールを確認し、不足していたら `BX.Build.cs` を更新すること**。更新後は必ずプロジェクトファイル再生成が必要。

---

## 10. Claude Code への依頼受付ルール

### 10-1. 依頼の解釈順序

ユーザーの依頼を受けたら、以下の順で処理してください:

1. 依頼文に章番号(§X-Y)があれば、**まず `docs/SPEC.md` を `view` で読む**
2. 影響範囲を確認(`git status`、関連ファイルの `view`)
3. 変更規模が大きい場合(5ファイル以上 or 新規クラス3つ以上)は、先にプランを提示して承認を得る
4. 実装 → ビルド試行(環境があれば)→ 動作確認指示を出力
5. 必要に応じて CLAUDE.md / SPEC.md の更新を提案

### 10-2. 曖昧な依頼への対応

**悪い依頼の例とその対処**:

> ユーザー: 「商人ハブを作って」

対処: 章番号が不明。以下のように返す:

> SPEC の §29(商人ハブ UI ワイヤー)を実装対象として想定していますが、以下のどれから着手しますか?
> A. §29-3 ホーム画面の C++ 基底ウィジェット
> B. §29-4 預かり在庫画面の構造体とロジック
> C. §29 全体のディレクトリ構造とプレースホルダを先に作る

### 10-3. 実装計画の標準フォーマット

大きな変更時は以下で計画を提示:

```markdown
## 実装計画: <タスク名>

### 対象仕様
- SPEC.md §X-Y: <要約>

### 変更ファイル
- 新規: Source/BX/Public/.../XXX.h
- 新規: Source/BX/Private/.../XXX.cpp
- 編集: BX.Build.cs(依存モジュール追加: "AIModule")

### Blueprint 作業(ユーザー対応)
- Content/BX/Characters/ に BP_BX_Player を APlayerCharacterBase から派生
- 詳細手順は実装完了後に手順書として出力

### 所要トークン見積もり
- 約 XX,XXX token(大中小)

この計画で進めてよろしいですか?
```

### 10-4. Blueprint 手順書の標準フォーマット

C++ 完了後に BP 作業を依頼するときは:

```markdown
## Blueprint 作業手順: BP_BX_Player

1. UE5 エディタを起動
2. `/Content/BX/Characters/` へ移動
3. 右クリック → Blueprint Class
4. 親クラス検索で `PlayerCharacterBase` を選択 → Select
5. 名前を `BP_BX_Player` に変更
6. BP を開く → 以下のコンポーネントとアセットを設定:
   - Mesh(SkeletalMesh): `SK_BX_Player`(まだ無ければプレースホルダで OK)
   - First Person Camera: Transform Z = +64.0
   - ...
7. Save All(Ctrl+Shift+S)
```

---

## 11. Git 運用

- ブランチ構成:
  - `main`: 常にビルドが通る状態
  - `develop`: 統合用
  - `feature/<機能>`: 機能開発(例: `feature/player-core`, `feature/merchant-hub`)
- コミット前に必ず `git status` を確認
- コミット粒度: 1 章の仕様追加 / 1 機能実装 = 1 コミット
- コミットメッセージは Conventional Commits(`feat:`, `fix:`, `refactor:`, `chore:`, `docs:`, `perf:`)
- LFS 対象: `.uasset`, `.umap`, 画像, 音声, FBX 等(`.gitattributes` で設定済)

### 11-1. コミット前チェック

C++ 変更をコミットする前に以下を案内:

```powershell
# 1. ビルドが通ることを確認
# BX.uproject を右クリック → Generate Visual Studio project files
# VS2022 で Development Editor / Win64 ビルド

# 2. git status で差分確認
git status

# 3. コミット
git add <file1> <file2>
git commit -m "feat(BX): §X-Y の <機能> を実装"
```

---

## 12. ビルド手順

### 12-1. C++ 追加/変更後

```
1. UE5 エディタを閉じる(開いたままだと反映されにくい)
2. BX.uproject 右クリック → "Generate Visual Studio project files"
3. BX.sln を VS2022 で開く
4. 構成: Development Editor / Win64
5. メニュー: ビルド → ソリューションのビルド(F7)
6. エラー 0 で完了したら UE5 エディタ再起動
```

### 12-2. CSV 更新後

```
1. UE5 エディタで対象 DT を開く(例: DT_BX_Weapons)
2. 右上の Reimport アイコン(🔄)をクリック
3. DataSource/weapons.csv を再読み込み
4. 行数と代表値が反映されていることを確認
5. Save All
```

**重要**: Live Coding は `UPROPERTY` 追加時は使わない(取りこぼす)。通常ビルドで。

---

## 13. SPEC.md との整合チェック

### 13-1. Claude Code がチェックすべきこと

SPEC.md を読んで実装するとき、以下に気づいたらユーザーに報告:

- データ項目表に型が書かれていない、または CLAUDE.md の型表記と異なる
- 範囲表記(例: 「4,000〜8,000」)があり、CSV 化できない
- 受け入れ条件がない、または曖昧
- 「前述の通り」「後述」など章番号で参照されていない記述
- `docs/SPEC_GAPS.md` の該当ギャップが未解決のまま

報告フォーマット:

```
⚠️ SPEC 整合性の指摘

§X-Y <章名> に以下の問題があります:
- <問題>
- 推奨: <対処案>

このまま実装を進めますか?それとも SPEC.md を先に更新しますか?
```

### 13-2. CLAUDE.md 自体の更新提案

実装中に「この規約を変えたほうが良い」と気づいた場合、**勝手に変えず**、以下のフォーマットで提案:

```
💡 CLAUDE.md 変更提案

現在の規約: <該当箇所>
提案: <変更案>
理由: <なぜ変えるべきか>

承認いただければ CLAUDE.md を更新します。
```

---

## 14. 参考リンク

- [UE5.4 Documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-5-4-documentation)
- [Enhanced Input](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine)
- [GameplayTag](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-tags-in-unreal-engine)
- [World Partition](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine)
- [PCG Framework](https://dev.epicgames.com/documentation/en-us/unreal-engine/procedural-content-generation-framework-in-unreal-engine)
- [Data Table Workflow](https://dev.epicgames.com/documentation/en-us/unreal-engine/data-driven-gameplay-elements-in-unreal-engine)

---

## 15. 最終チェックリスト(作業完了時)

C++ 実装タスクを完了したと判断する前に、以下を自己確認:

- [ ] SPEC.md の該当章を読んだ
- [ ] 新規 C++ ファイルは `Source/BX/Public` + `Source/BX/Private` のペアで配置
- [ ] `UPROPERTY` / `UFUNCTION` に `Category="BX|..."` が付いている
- [ ] ポインタ型は `TObjectPtr` / `TSoftObjectPtr` / `TWeakObjectPtr` を適切に使い分け
- [ ] 必要なら `BX.Build.cs` の依存モジュール追加
- [ ] Include 順序が正しい(`*.generated.h` が最後)
- [ ] 新規 Enum / Struct は BP 公開用に `BlueprintType` 付与
- [ ] CSV を更新した場合はユーザーに Reimport 手順を伝えた
- [ ] 関連する BP 作業が必要なら手順書を出力した
- [ ] コミットメッセージの案を提示した

以上。疑問点があれば作業開始前にユーザーに質問してください。
