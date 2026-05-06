\# Sprint 18 PLAN — HUD 実装(最小機能版)



\## 目的



§16 HUD 仕様の最初の柱として、プレイヤーが画面で:

\- 体力ゲージ × 7 部位(Head, Chest, Abdomen, LeftArm, RightArm, LeftLeg, RightLeg)

\- 弾薬残数(現在弾数 / マガジン容量)

\- クロスヘア



を見られる WBP\_BX\_HUD を実装する。



PIE で「ダミーを撃つ → 弾薬数が減る、リロードで回復、ダミー死亡で武器持ち替え可」が画面で確認できるところまでを完了基準とする。



\## スコープ外(Sprint 19 以降)



\- 状態異常アイコン(出血、骨折)

\- 防具耐久度表示

\- 装備中アイテムスロット表示

\- ミニマップ

\- 設定画面 / インベントリ画面

\- アイコンの差し替え(Sprint 18 ではプリミティブ図形 or デフォルトテクスチャで OK)

\- アニメーション(ダメージ受けた時の赤フラッシュなど)



\## 参照



\- docs/SPEC\_PartB.md §16 HUD 仕様(該当章があれば)

\- 既存実装: Source/BX/Public/Characters/Components/AC\_BX\_HealthBodyParts.h

\- 既存実装: Source/BX/Public/Characters/Components/AC\_BX\_WeaponHandler.h

\- 既存実装: Source/BX/Public/Characters/APlayerCharacterBase.h

\- 既存実装: Source/BX/Public/Data/BXEnums.h(EBXBodyPart)



\## Sprint 18 スコープ



\### 1. UBXHUDWidget(C++ 基底クラス)



ファイル:

\- Source/BX/Public/UI/BXHUDWidget.h(新規)

\- Source/BX/Private/UI/BXHUDWidget.cpp(新規)



UCLASS: UUserWidget 派生



メンバ:

\- TWeakObjectPtr<AAPlayerCharacterBase> OwningPlayer

\- 公開関数:

&#x20; - SetOwningPlayer(APlayerCharacterBase\* Player)

&#x20; - UpdateBodyPartHP(EBXBodyPart Part, float HPRatio)(0.0〜1.0)

&#x20; - UpdateAmmo(int32 Current, int32 MagSize)

&#x20; - UpdateCurrentWeaponName(FName WeaponRowName)



実装方針:

\- Tick または Timer で OwningPlayer から HP / 弾薬を取得

\- BlueprintImplementableEvent で BP 側に通知(Widget Blueprint で UI 更新)

\- 仮想関数として OnUpdateBodyPartHP, OnUpdateAmmo を BP override 可能に



\### 2. APlayerCharacterBase の HUD 連携



既存ファイル:

\- Source/BX/Public/Characters/APlayerCharacterBase.h

\- Source/BX/Private/Characters/APlayerCharacterBase.cpp



追加:

\- TSubclassOf<UBXHUDWidget> HUDWidgetClass(EditDefaultsOnly、BP\_BX\_Player でアサイン)

\- TObjectPtr<UBXHUDWidget> HUDWidgetInstance

\- BeginPlay で:

&#x20; - if HUDWidgetClass: CreateWidget → AddToViewport → SetOwningPlayer(this)

\- Tick で(または Timer):

&#x20; - HealthBodyPartsComponent から各部位 HP を取得 → HUDWidget->UpdateBodyPartHP

&#x20; - WeaponHandlerComponent から現在弾薬を取得 → HUDWidget->UpdateAmmo



\### 3. WBP\_BX\_HUD(Widget Blueprint、ユーザーが UE5 で作成)



ユーザー側で作成:

\- Content/BX/UI/Widgets/WBP\_BX\_HUD.uasset

\- 親クラス: UBXHUDWidget(C++ 基底)



Widget レイアウト:

\- Canvas Panel(ルート)

&#x20; - 左下: 体力ゲージ × 7 部位(ProgressBar、縦に並べる)

&#x20;   - Head, Chest, Abdomen, LeftArm, RightArm, LeftLeg, RightLeg

&#x20;   - ラベル + ProgressBar

&#x20; - 右下: 弾薬テキスト("29 / 30" 形式)

&#x20; - 中央: クロスヘア(Image または Border、十字 or 点)



C++ から呼ばれる関数を BP で override:

\- OnUpdateBodyPartHP(EBXBodyPart, float Ratio): 該当部位の ProgressBar を更新

\- OnUpdateAmmo(Current, MagSize): テキスト更新



\### 4. ビルド検証



ビルドが通ることを必ず確認。



\## Sprint 18 完了基準



\- UBXHUDWidget が実装されている

\- APlayerCharacterBase に HUDWidgetClass + HUDWidgetInstance が追加

\- BeginPlay で HUD が AddToViewport される

\- Tick で HUD が更新される(HP, 弾薬)

\- ビルドが通る(必須)

\- docs/SPRINT\_REPORT\_2026-04-24.md に Sprint 18 実行結果を追記



\## Sprint 18 でやらないこと



\- アイコンテクスチャの作成(プログラマブル ProgressBar / Text のみで OK)

\- 状態異常表示(Sprint 20)

\- 防具耐久度表示(Sprint 19)

\- 武器スロット表示(Sprint 19)

\- インベントリ画面(Sprint 22)

\- 設定画面(Sprint 24+)

\- メニュー画面(Sprint 24+)

\- HUD アニメーション(ダメージフラッシュなど)



\## ユーザー側の作業(C++ 完了後、UE5 エディタで)



\### Phase 1: WBP\_BX\_HUD 作成



1\. Content Browser で `Content/BX/UI/Widgets/` フォルダを作成(なければ)

2\. 右クリック → User Interface → Widget Blueprint

3\. 親クラス選択ダイアログで \*\*`BXHUDWidget`\*\*(または C++ クラス名)を選択

4\. 名前: \*\*`WBP\_BX\_HUD`\*\*



\### Phase 2: HUD のレイアウト作成



1\. WBP\_BX\_HUD をダブルクリック

2\. Hierarchy パネルで Canvas Panel が ルート

3\. 以下を配置:



\#### 体力ゲージ × 7 部位(左下)

\- Vertical Box を配置(左下にアンカー)

\- 中に 7 個の Horizontal Box:

&#x20; - 各 HBox に Text(部位名)+ ProgressBar

\- 各 ProgressBar の名前を `PB\_HP\_Head`, `PB\_HP\_Chest`, ..., `PB\_HP\_RightLeg` に

\- 「変数」チェック ON(C++ から参照可能に)



\#### 弾薬テキスト(右下)

\- Text Block を配置

\- 名前: `Text\_Ammo`、変数 ON

\- デフォルトテキスト: "30 / 30"



\#### クロスヘア(中央)

\- Border または Image を配置

\- 中央アンカー、サイズ 4x4 px の点

\- 色: 白 or 緑



\### Phase 3: グラフで関数 override



1\. グラフタブに切り替え

2\. \*\*OnUpdateBodyPartHP\*\* イベント追加(C++ の BlueprintImplementableEvent)

&#x20;  - 入力: EBXBodyPart Part, float Ratio

&#x20;  - Switch on EBXBodyPart で各部位の ProgressBar を Set Percent(Ratio)

3\. \*\*OnUpdateAmmo\*\* イベント追加

&#x20;  - 入力: int32 Current, int32 MagSize

&#x20;  - Text\_Ammo.SetText(Format Text "{0} / {1}")



\### Phase 4: BP\_BX\_Player に HUDWidgetClass アサイン



1\. BP\_BX\_Player を開く → Class Defaults

2\. 検索 `HUDWidgetClass` または `HUD Widget Class`

3\. ドロップダウンから \*\*`WBP\_BX\_HUD`\*\* を選択

4\. コンパイル + Ctrl+S



\### Phase 5: PIE で動作確認



1\. PIE 起動

2\. 画面に体力ゲージ 7 個 + 弾薬テキスト + クロスヘアが表示されているか

3\. 武器拾う → 弾薬テキストが "30 / 30" になる

4\. 撃つ → 弾薬テキストが減る

5\. ダミーを撃つ → 自分の HP は変わらない(自分が撃たれてないから)

6\. もしダミーから撃ち返される機能なし → 自分の HP が変わるテストは Sprint 25 以降

7\. リロード → 弾薬数が回復



\## 報告(Claude Code 用)



完了時に以下を報告:

1\. 作成 / 更新したファイルパスのリスト

2\. 追加した関数のリスト

3\. ビルド成功 / 失敗(失敗時はエラーメッセージ)

4\. UI モジュール依存の追加が必要だった場合(BX.Build.cs に "UMG", "Slate", "SlateCore" 追加)

5\. ユーザーが UE5 エディタでやるべき作業(箇条書き、特に WBP\_BX\_HUD 作成手順を詳細に)

6\. 次の Sprint(Sprint 19)の推奨内容(防具アイテム化 or 状態異常)



最後に commit + push をしてください。

コミットメッセージ: feat: Sprint 18 - HUD 基盤実装(UBXHUDWidget + APlayerCharacterBase HUD 連携)

