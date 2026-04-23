# DataSource/

Data Table にインポート前の CSV / JSON を置きます。

## 命名ルール

- `items.csv` → UE5 側の `DT_Items` に対応
- `enemies.csv` → `DT_Enemies` に対応
- CSV は UTF-8 (BOM なし) で保存

## インポート手順

1. UE5 エディタで Content/DataTables/ の対象 Data Table を開く
2. 右上の **Reimport**(🔄)をクリック
3. 差分を確認して保存

## Claude Code への依頼例

> `FItemRow` 構造体の定義を見て、`DataSource/items.csv` に武器カテゴリのサンプルを 20 件追加してください。
