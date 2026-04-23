# 仕様書テンプレート(ChatGPT との共同作成用)

このテンプレートをコピーして `docs/SPEC.md` として使います。
**Claude Code が読みやすく、タスク分解しやすい構造**に最適化してあります。

---

## 執筆ルール(ChatGPT にもこれを伝える)

1. **各セクションに章番号**(§1, §1.1…)を振る。Claude Code に「§3.2 を実装」と指示できるように。
2. **データ項目は必ず表形式**で書く(列: 名称 / 型 / 必須 / 説明)。Claude Code が Data Table 構造体に直訳できる。
3. **UI画面は Widget Blueprint 単位**で章を分ける(§UI-01 メインメニュー 等)。
4. **「Blueprint で作る」「C++ で作る」の方針を明示**。迷ったら「まず Blueprint、ボトルネック出たら C++」と書く。
5. **受け入れ条件(Acceptance Criteria)** を各機能に 3〜5 箇条書きで付ける。

---

# `<PROJECT_NAME>` 仕様書

## §1. ゲーム概要

- **タイトル**: `<PROJECT_NAME>`
- **ジャンル**: `<GENRE>`
- **ターゲット**: `<対象プレイヤー>`
- **プレイ時間**: 1セッション `<X>` 分
- **プラットフォーム**: Windows (Steam)
- **視点 / 操作**: `<例: 3Dサードパーソン / WASD + マウス>`

### §1.1 コアループ

```
<1行で: プレイヤーが何を繰り返すゲームか>
例: 「ダンジョンに潜る → アイテム収集 → 脱出 → 装備強化 → より深いダンジョンへ」
```

### §1.2 コアメカニクス(3つまで)

1. `<メカニクス1>`
2. `<メカニクス2>`
3. `<メカニクス3>`

---

## §2. システム構成

### §2.1 GameMode / GameInstance / GameState の責務分担

| クラス | 責務 | C++ / BP |
|---|---|---|
| `AGameModeBase_<Project>` | ルール、勝敗判定、スポーン制御 | C++ 基底 + BP 派生 |
| `UGameInstance_<Project>` | セッション跨ぎの状態、セーブ管理 | C++ |
| `AGameStateBase_<Project>` | レプリケート対象の状態(単体でも使う) | C++ 基底 + BP 派生 |

### §2.2 主要クラス階層

```
APawn
└─ ACharacter
   └─ APlayerCharacterBase (C++) ← カメラ・Enhanced Input ここで実装
      └─ BP_PlayerCharacter (BP)  ← メッシュ・アニメ・音

AActor
└─ AInteractableBase (C++, interface 実装)
   ├─ BP_Chest
   └─ BP_Door
```

---

## §3. ゲームプレイ機能

### §3.1 プレイヤー操作

| アクション | 入力(PC) | Input Action | 実装 |
|---|---|---|---|
| 移動 | WASD | `IA_Move` | C++(PlayerCharacterBase) |
| 視点 | Mouse | `IA_Look` | C++ |
| ジャンプ | Space | `IA_Jump` | C++ |
| インタラクト | E | `IA_Interact` | C++ → BP でイベント |
| ポーズ | Esc | `IA_Pause` | BP(Widget 経由) |

**受け入れ条件**:
- [ ] 全アクションが `IMC_Default` 経由で動作する
- [ ] 操作はパッドにも対応する(分岐は Mapping Context で)
- [ ] キーコンフィグは将来的に `DA_InputBindings` で差し替え可能にする

### §3.2 `<機能名>`(例: インベントリ)

**目的**: `<なぜ必要か>`

**仕様**:
- `<箇条書きで定義>`

**データ構造**:

```cpp
// Data Table 行構造体
USTRUCT(BlueprintType)
struct FItemRow : public FTableRowBase
{
    // 下の表と一致させること
};
```

| 列名 | 型 | 必須 | 説明 |
|---|---|---|---|
| `ItemId` | FName | ✅ | 主キー |
| `DisplayName` | FText | ✅ | UI 表示名 |
| `BasePrice` | int32 | ✅ | 基本価格 |
| `Icon` | TSoftObjectPtr&lt;UTexture2D&gt; |  | アイコン |

**CSV サンプル** (`DataSource/items.csv`):
```csv
Name,DisplayName,BasePrice,Icon
sword,"鉄の剣",100,/Game/UI/Icons/T_sword
potion,"回復薬",50,/Game/UI/Icons/T_potion
```

**UI**: `WBP_Inventory`(§UI-03 参照)

**受け入れ条件**:
- [ ] `DT_Items` に 10 件以上のサンプルデータがある
- [ ] アイテム取得時にインベントリに追加され、UI に反映される
- [ ] セーブ対象に含まれる

---

## §4. UI(Widget Blueprint)

### §UI-01 メインメニュー (`WBP_MainMenu`)

**ボタン**:
- はじめから
- つづきから(SaveGame がある場合のみ有効)
- 設定
- 終了

### §UI-02 HUD (`WBP_HUD`)

**要素**:
- 左上: HP バー
- 右下: ミニマップ
- 中央: インタラクトプロンプト(対象がある時のみ)

### §UI-03 インベントリ (`WBP_Inventory`)

...

---

## §5. データアセット一覧

| 資産 | 種別 | パス | 用途 |
|---|---|---|---|
| `DT_Items` | Data Table | `/Game/DataTables/` | アイテムマスタ |
| `DT_Enemies` | Data Table | `/Game/DataTables/` | 敵マスタ |
| `DA_GameBalance` | Data Asset | `/Game/DataAssets/` | 難易度・成長曲線 |

---

## §6. セーブデータ設計

### §6.1 スロット構成

- スロット名: `AutoSave`, `Manual_01` 〜 `Manual_03`
- オートセーブ契機: ステージ開始時 / ボス撃破時

### §6.2 セーブ構造体

```cpp
UCLASS()
class USaveGame_<Project> : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY() int32 SaveVersion = 1;  // マイグレーション用
    UPROPERTY() FDateTime SavedAt;
    UPROPERTY() FName CurrentLevelName;
    UPROPERTY() TArray<FSavedItem> Inventory;
    UPROPERTY() int32 Gold = 0;
    // ...
};
```

**JSON 補助**: デバッグ時のみ `Saved/DebugDumps/save.json` に書き出し可能にする。

---

## §7. マイルストーン

| M | 内容 | 期限 |
|---|---|---|
| M0 | プロジェクトセットアップ | `<date>` |
| M1 | プレイヤー操作 + テストステージ | `<date>` |
| M2 | コアループ1周できる | `<date>` |
| M3 | UI 一通り動く | `<date>` |
| M4 | セーブ / ロード | `<date>` |
| M5 | サウンド・VFX 仮実装 | `<date>` |
| M6 | ベータ(外部テスト) | `<date>` |
| M7 | リリース候補 | `<date>` |

---

## §8. 未決事項(TBD)

- `<まだ決まっていないことをここに書く>`
- `<ChatGPT と議論する項目>`
