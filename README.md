# `<PROJECT_NAME>` — UE5 + Claude Code プロジェクト

Unreal Engine 5 + Claude Code で開発する個人ゲームプロジェクトのスキャフォールドです。

## クイックスタート

```powershell
# 1. Claude Code 起動(プロジェクトルートで)
claude

# 2. 仕様書を更新
#    docs/SPEC.md を ChatGPT と一緒に書く

# 3. 実装依頼
#    「docs/SPEC.md の §3.2 のインベントリ機能を実装して」
#    のように仕様書の章を指定して依頼する
```

## ドキュメント

| ファイル | 内容 |
|---|---|
| [`CLAUDE.md`](./CLAUDE.md) | **Claude Code が自動で読む**プロジェクト規約 |
| [`docs/SPEC.md`](./docs/SPEC.md) | 仕様書(ChatGPT と作成) |
| [`docs/SPEC_TEMPLATE.md`](./docs/SPEC_TEMPLATE.md) | 仕様書テンプレート |
| [`docs/CONVENTIONS.md`](./docs/CONVENTIONS.md) | コード規約・命名規則 |
| [`docs/WORKFLOW.md`](./docs/WORKFLOW.md) | 日々の作業フロー |
| [`SETUP_GUIDE.md`](./SETUP_GUIDE.md) | 初回セットアップ手順 |

## 技術スタック

- **Engine**: Unreal Engine 5.x
- **主言語**: Blueprint(中心) + C++(必要に応じて)
- **データ**: Data Table / Data Asset
- **UI**: Widget Blueprint
- **Save**: SaveGame + JSON 補助
- **Input**: Enhanced Input System
