# 日々の作業フロー

## 基本ループ

```
[1] 仕様更新 ──→ [2] Claude Code に依頼 ──→ [3] C++ / データ更新
        ↑                                          │
        │                                          ▼
[6] 次の章へ ←── [5] Git commit ←── [4] UE5 エディタで BP 作成・検証
```

---

## 1. 仕様を書く / 更新する(ChatGPT 側)

ChatGPT と `docs/SPEC.md` を編集する。
**原則**: 1 コミット = 1 章の仕様追加 or 1 機能実装 まで。

### ChatGPT にコピペする前提プロンプト例

> 次の仕様書の章を書くのを手伝って。フォーマットは `docs/SPEC_TEMPLATE.md` に従い、必ず:
> - 章番号(§X.Y)を付ける
> - データ項目は表形式で(列: 名称 / 型 / 必須 / 説明)
> - 受け入れ条件を 3〜5 個付ける
>
> 今回書きたい章: §3.5 「敵 AI」

---

## 2. Claude Code に実装を依頼する

プロジェクト直下で `claude` 起動。初回の会話で CLAUDE.md が読み込まれる。

### 良い依頼の例 ✅

> `docs/SPEC.md` の §3.2(インベントリ)を C++ 側だけ実装してください。
> Blueprint 側の作業は手順書で出してください。

> §3.1 のプレイヤー操作について、Enhanced Input の Input Action と C++ の入力受付ロジックを追加してください。Mapping Context は DA_InputMappingContext で持ちます。

> `DataSource/items.csv` から `FItemRow` 構造体に合わせて 30 件のサンプルデータを生成してください。

### 悪い依頼の例 ❌

> BP_PlayerCharacter を作って

→ Claude Code は Blueprint を直接作れません。「**C++ 基底クラスを作って、そこから派生した BP の作り方を手順書で**」と依頼してください。

> インベントリを作って

→ 仕様書のどこを参照するか不明確。「§3.2 のインベントリ」のように指定する。

---

## 3. C++ / データファイルの更新

Claude Code が編集するのは基本:
- `Source/<PROJECT>/**/*.{h,cpp}`
- `Source/<PROJECT>/<PROJECT>.Build.cs`
- `Config/Default*.ini`
- `DataSource/*.csv`
- `docs/**/*.md`

### C++ を追加した後の手順

1. UE5 エディタを**閉じる**(開いたままだとクラス追加が反映しにくい)
2. プロジェクトルートの `.uproject` を**右クリック → Generate Visual Studio project files**
3. Visual Studio 2022 で **Development Editor / Win64** でビルド
4. UE5 エディタを再起動

### CSV を更新した後の手順

1. UE5 エディタで対応する Data Table(例: `DT_Items`)を開く
2. 右上の **Reimport**(🔄 マーク)をクリック
3. 差分が反映されていることを確認

---

## 4. UE5 エディタで Blueprint 作業

Claude Code が出した手順書に従って、エディタ側で:
- Blueprint Class の派生
- Widget Blueprint の配置
- Data Asset のインスタンス作成
- レベル配置

作業前後で必ず **Save All**(Ctrl+Shift+S)。

---

## 5. Git commit

**個人開発でも必ずコミット粒度を保つ**:

```powershell
# 仕様追加だけ
git add docs/SPEC.md
git commit -m "docs: add §3.5 enemy AI specification"

# 実装(仕様とは別コミット)
git add Source/ Config/
git commit -m "feat: add base enemy AI controller (§3.5)"

# アセット(BP作成後)
git add Content/
git commit -m "feat: add BP_GoblinAI derived from AAIControllerBase"
```

---

## 6. 困ったときの Claude Code への質問

- 「このビルドエラーを調査して」→ エラー全文を貼り付ける
- 「DT_Items の CSV と C++ 構造体の差分を確認して」
- 「§3.2 の受け入れ条件を今の実装が満たしているかレビューして」
- 「docs/SPEC.md の未決事項(§8)を洗い出して優先度付けして」

---

## チェックリスト(機能1つを完了する目安)

- [ ] `docs/SPEC.md` に仕様がある(章番号付き)
- [ ] C++ 基底クラスがある(必要なら)
- [ ] Blueprint 派生クラスが `Content/` にある
- [ ] Data Table / Data Asset にサンプルデータがある
- [ ] 仕様の受け入れ条件を満たしている
- [ ] SaveGame 対象なら `USaveGame_<Project>` に追加済み
- [ ] ビルドが通る / PIE で動作確認済み
- [ ] コミット済み
