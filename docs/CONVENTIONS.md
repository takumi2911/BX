# コード規約・命名規則

`CLAUDE.md` §4 の補足です。

---

## 1. C++ 規約

### 1.1 ヘッダの基本構造

```cpp
// Copyright (c) <year> <PROJECT_NAME>. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

// Forward declarations(可能な限りインクルードを減らす)
class UStaticMeshComponent;

UCLASS()
class MYPROJECT_API AMyActor : public AActor
{
    GENERATED_BODY()

public:
    AMyActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(ClampMin="0.0"))
    float Health = 100.0f;
};
```

### 1.2 `UPROPERTY` / `UFUNCTION` のルール

- **Blueprint に公開する値は必ず `Category` を付ける**(Blueprint のノードが整理される)
- `meta=(ClampMin=, ClampMax=)` で入力範囲を制約
- ポインタ型のメンバは UE5 推奨の **`TObjectPtr<T>`** を使う(`T*` より GC 安全)
- 遅延ロードは **`TSoftObjectPtr<T>`**、参照だけしたいときは **`TWeakObjectPtr<T>`**

### 1.3 Include の順序

1. 対応するヘッダ(.cpp の最初)
2. UE5 フレームワーク(`CoreMinimal.h`, `GameFramework/...`)
3. 自プロジェクト内ヘッダ
4. `<YourClass>.generated.h` は **必ず最後**

---

## 2. Blueprint 規約

### 2.1 ノード配置

- **左上から右下に流れる配置**(UE5 エディタの自動整列に合わせる)
- コメントボックスで機能単位にグルーピング
- 10ノード以上の関数は関数化 or Macro 化

### 2.2 変数

- 命名は **`PascalCase`**
- 種別プレフィックスは**付けない**(`bIsAlive` のような bool の `b` は例外として許可)
- `Category` を設定してメニューを整理
- 外部変更可の変数はインスタンス編集可にする

### 2.3 BP → C++ 昇格の判断

以下のいずれかに該当したら C++ 化を検討:
- Tick で毎フレーム走る重い処理
- ネストが深すぎて可読性が落ちた
- 他の BP 3つ以上が同じロジックをコピペしている
- プロファイラで BP VM がボトルネックに出た

---

## 3. アセット配置ルール

- Content 配下では `/Game/<機能カテゴリ>/<種別>/` の深さで収める(3階層目安)
- 共通で使うものは `/Game/Core/` or `/Game/Shared/`
- 使い捨てテスト用は `/Game/_Sandbox/` に集約し、定期的に掃除

---

## 4. Git 運用

### 4.1 ブランチ

- `main`: 常にビルドが通る状態
- `feat/<機能名>`: 機能開発
- `fix/<内容>`: バグ修正
- `chore/<内容>`: 設定・ドキュメント

### 4.2 コミットメッセージ(Conventional Commits)

```
<type>: <概要>

<本文(任意)>
```

| type | 用途 |
|---|---|
| `feat` | 機能追加 |
| `fix` | バグ修正 |
| `refactor` | 振る舞い変えないリファクタ |
| `chore` | ビルド・設定・依存関係 |
| `docs` | ドキュメント |
| `style` | フォーマット |
| `perf` | 性能改善 |

### 4.3 アセットロック

個人開発なので基本不要だが、将来的に共同作業する場合は File Locking を有効化:
```
git lfs lock Content/Path/To/Asset.uasset
```

---

## 5. パフォーマンス指針

- **Tick を安易に使わない**。タイマー / イベント駆動を優先。
- Actor 数 10,000 超は Instanced Static Mesh / Niagara を検討
- ブループリントでの文字列結合はループで使わず、StringBuilder 相当のパターンで書く
- Data Table は起動時にまとめてロードし、以降はキャッシュ参照
