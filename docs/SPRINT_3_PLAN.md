# 実装計画: Sprint 3 — SaveGame 構造体

## 対象仕様
- CLAUDE.md §6: 6 ブロック構成 + 各フィールド方針
- SPEC_GAPS.md G-01: FBXProfileMeta / FBXMerchantNetworkState / FBXMerchantState の具体フィールド
- SPEC_PartB.md §16-3: 未収録（SPEC_GAPS.md で代替）

## ⚠️ 注記
§16-3 は SPEC_PartB.md に未収録のため、SPEC_GAPS.md G-01 と CLAUDE.md §6 で確認できる範囲のみ実装し、
不明フィールドは docs/QUESTIONS.md に記録した。

## 変更ファイル

| 種別 | パス |
|---|---|
| 新規 | `Source/BX/Public/Save/FBXProfileMeta.h` |
| 新規 | `Source/BX/Public/Save/FBXGlobalSettings.h` |
| 新規 | `Source/BX/Public/Save/FBXMerchantNetworkState.h` |
| 新規 | `Source/BX/Public/Save/FBXPlayerProgress.h` |
| 新規 | `Source/BX/Public/Save/FBXRegionState.h` |
| 新規 | `Source/BX/Public/Save/FBXRunSnapshot.h` |
| 新規 | `Source/BX/Public/Save/USaveGame_BX.h` |
| 新規 | `Source/BX/Private/Save/USaveGame_BX.cpp` |
| 編集 | `Source/BX/BX.Build.cs` ("Json", "JsonUtilities" 追加) |
| 新規 | `docs/QUESTIONS.md` (未確定フィールドの記録) |

## 構造体の依存関係

```
FBXSavedItemStack  ← FBXMerchantNetworkState.h 内に定義
FBXMerchantState   ← FBXMerchantNetworkState.h 内に定義
                      └─ TArray<FBXSavedItemStack>
FBXMerchantNetworkState
  └─ TMap<FName, FBXMerchantState>
  └─ TArray<FName> UnlockedRespawnNodes

FBXRunSnapshot
  └─ FBXSavedItemStack の再利用（別 include）

USaveGame_BX
  └─ 上記 6 構造体をすべて包含
  └─ DebugDumpJson() 関数（Json/JsonUtilities モジュール利用）
```

## 未確定フィールド (→ docs/QUESTIONS.md に記録)
- FBXGlobalSettings のキーコンフィグ構造
- FBXPlayerProgress の依頼進行フィールド詳細
- FBXRegionState の地域ごとのイベント状態詳細
- FBXRunSnapshot の保持インベントリ詳細

## ビルド結果
- (実装後に記載)
