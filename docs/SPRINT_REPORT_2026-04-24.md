# スプリント実行レポート 2026-04-24

## 実行概要

| 項目 | 内容 |
|---|---|
| 開始時刻 | 2026-04-24（セッション内・無人実行） |
| 完了時刻 | 同日 |
| 実行者 | Claude Sonnet 4.6（無人モード） |

---

## 各スプリントの状態

| スプリント | 状態 | ビルド |
|---|---|---|
| Sprint 1: Enum 拡張（事前済み） | ✅ 完走 | 成功（エラー 0） |
| Sprint 2: 武器 / 弾薬 DT 行構造体 | ✅ 完走 | 成功（エラー 0） |
| Sprint 3: SaveGame 構造体 | ✅ 完走 | 成功（エラー 0） |
| Sprint 4: インベントリ基本構造 | ✅ 完走 | 成功（エラー 0） |

---

## 作成・変更ファイル一覧

### Sprint 1 仕上げ（feature/sprint-1-enums → main マージ）
| 操作 | ファイル |
|---|---|
| 編集（済み） | `Source/BX/Public/Data/BXEnums.h` |

### Sprint 2（feature/sprint-2-weapons → main マージ）
| 操作 | ファイル |
|---|---|
| 新規 | `Source/BX/Public/Data/FBXWeaponTableRow.h` |
| 新規 | `Source/BX/Public/Data/FBXAmmoTableRow.h` |
| 新規 | `DataSource/weapons.csv`（サンプル 3 行） |
| 新規 | `DataSource/ammo.csv`（代表弾 11 種） |
| 編集 | `Source/BX/BX.Build.cs`（Niagara 追加） |
| 新規 | `docs/SPRINT_2_PLAN.md` |

### Sprint 4（feature/sprint-4-inventory → main マージ）
| 操作 | ファイル |
|---|---|
| 新規 | `Source/BX/Public/Items/FBXInventoryTypes.h`（FBXInventorySlotDef + FBXInventoryItemStack） |
| 新規 | `Source/BX/Public/Data/FBXItemTableRow.h` |
| 新規 | `Source/BX/Public/Items/Components/AC_BX_Inventory.h` |
| 新規 | `Source/BX/Private/Items/Components/AC_BX_Inventory.cpp` |
| 新規 | `DataSource/items.csv`（25 行） |
| 編集 | `Source/BX/Public/Save/FBXMerchantNetworkState.h`（FBXSavedItemStack → FBXInventoryItemStack） |
| 編集 | `Source/BX/Public/Save/FBXRunSnapshot.h`（同上） |
| 編集 | `docs/QUESTIONS.md`（Q-02 クローズ） |
| 新規 | `docs/SPRINT_4_PLAN.md` |

### Sprint 3（feature/sprint-3-savegame → main マージ）
| 操作 | ファイル |
|---|---|
| 新規 | `Source/BX/Public/Save/FBXProfileMeta.h` |
| 新規 | `Source/BX/Public/Save/FBXGlobalSettings.h` |
| 新規 | `Source/BX/Public/Save/FBXMerchantNetworkState.h` |
| 新規 | `Source/BX/Public/Save/FBXPlayerProgress.h` |
| 新規 | `Source/BX/Public/Save/FBXRegionState.h` |
| 新規 | `Source/BX/Public/Save/FBXRunSnapshot.h` |
| 新規 | `Source/BX/Public/Save/USaveGame_BX.h` |
| 新規 | `Source/BX/Private/Save/USaveGame_BX.cpp` |
| 編集 | `Source/BX/BX.Build.cs`（Json / JsonUtilities 追加） |
| 新規 | `docs/SPRINT_3_PLAN.md` |
| 新規 | `docs/QUESTIONS.md` |

---

## ビルド結果

| スプリント | アクション数 | 結果 |
|---|---|---|
| Sprint 1 | 6 | ✅ 成功、エラー 0 |
| Sprint 2 | 8 | ✅ 成功、エラー 0 |
| Sprint 3 | 16 | ✅ 成功、エラー 0 |
| Sprint 4 | 16 | ✅ 成功、エラー 0 |

---

## git log（本日セッション分）

```
712a48e merge: sprint 4 inventory base
2a83b3a feat(BX): §15-1 FBXItemTableRow / FBXInventoryTypes / AC_BX_Inventory を実装・FBXSavedItemStack を統合
3945df0 docs: スプリント 1〜3 実行レポートと QUESTIONS.md を追加
8e99035 merge: sprint 3 savegame structs
b7f3bda feat(BX): §16-3(SPEC_GAPS G-01) SaveGame 構造体 6 本と USaveGame_BX を実装
d2fa0c9 merge: sprint 2 weapon/ammo DT row structs
a4e6d93 feat(BX): §14-4/§14-5-2 FBXWeaponTableRow・FBXAmmoTableRow 行構造体と CSV スケルトンを追加
08a9713 merge: sprint 1 enum expansion
bef5f41 feat(BX): §14-5/§15-1/§15-2/§18-2/§21-1/§21-2/§29-4 の Enum を定義・拡張
```

---

## 残件（ユーザーがレビューすべき箇所）

### 1. UE5 エディタでの DataTable 作成（ユーザー作業）

Sprint 2 のヘッダ実装後、以下の手順でエディタ作業が必要です。

```
【DT_BX_Weapons の作成】
1. /Content/BX/Data/Common/Weapons/ フォルダを新規作成
2. 右クリック → Miscellaneous → Data Table
3. Row Structure: FBXWeaponTableRow → 名前: DT_BX_Weapons → Save
4. DT_BX_Weapons を開き Reimport → DataSource/weapons.csv を指定

【DT_BX_Ammo の作成】
5. /Content/BX/Data/Common/Ammo/ フォルダを新規作成
6. 同様に FBXAmmoTableRow で DT_BX_Ammo を作成 → DataSource/ammo.csv をインポート
7. Save All
```

### 2. §16-3 の SPEC 執筆（ChatGPT への依頼）

Sprint 3 の SaveGame 実装は SPEC_GAPS.md G-01 を根拠としていますが、§16-3 が
SPEC_PartB.md に未収録のため、以下のフィールドが省略・簡略化されています:

- `FBXGlobalSettings.KeyConfig`（キーコンフィグ構造）
- `FBXPlayerProgress.ActiveQuestIds`（依頼進行が商人側と重複するか未定）
- `FBXRegionState` のイベント状態詳細
- `FBXRunSnapshot` の中断セーブ発動条件

詳細は `docs/QUESTIONS.md` の Q-01〜Q-05 を参照してください。

### ~~3. `FBXSavedItemStack` と §15-1 `FBXInventoryItemStack` の統合判断~~ ✅ Sprint 4 で解決

`FBXSavedItemStack` を廃止し `FBXInventoryItemStack`（§15-1-2）に統合済み。
`FBXMerchantNetworkState.h` / `FBXRunSnapshot.h` の両ファイルを更新した。

### 4. weapons.csv の `FireModes` 列について

UE5 の DataTable インポートで `TArray<EBXFireMode>` 型の列は、
インポート時の区切り文字の扱いがエディタバージョンにより異なる場合があります。  
インポート後に FireModes が正しく入力されているか確認してください。  
問題がある場合は CSV の `"FullAuto|SemiAuto"` 形式を調整が必要です。

---

## docs/QUESTIONS.md の要点

Q-01: キーコンフィグの保存構造が未定義  
Q-02: FBXSavedItemStack と FBXInventoryItemStack の統合判断が必要  
Q-03: 依頼進行の管理場所（商人別 vs グローバル）が未定  
Q-04: FBXRegionState のイベント詳細フィールドが未定  
Q-05: レイド中断セーブの発動条件（モード別可否）が未定  

---

## 最終 git status

```
On branch main
nothing to commit, working tree clean
（.claude/ は .gitignore 対象外の未追跡フォルダのみ）
```
