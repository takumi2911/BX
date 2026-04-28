\# Sprint 13 PLAN — AC\_BX\_PlayerInteraction 実装



\## 目的



§23-7 プレイヤー Interaction / Loot / Extract 連携の最初の柱として、

プレイヤーが「拾う・ドアを開ける・調べる」ためのコンポーネント

`UAC\_BX\_PlayerInteraction` を C++ で実装する。



PIE で「E キー押下 → 床のテストアイテムをインベントリに追加」が動くところまでを完了基準とする。



\## 参照



\- docs/SPEC\_PartB.md §23-7 プレイヤー Interaction / Loot / Extract 連携

\- docs/SPEC\_PartB.md §15-1 アイテム DataTable とインベントリ基本構造

\- docs/SPEC\_PartB.md §18-3 抽出ポイント Actor 基底仕様

\- 既存実装: Source/BX/Public/Items/Components/UAC\_BX\_Inventory.h

\- 既存スタブ: Source/BX/Public/Characters/Components/AC\_BX\_PlayerInteraction.h



\## Sprint 13 スコープ



\### 1. IInteractable インターフェース作成



ファイル:

\- Source/BX/Public/Interaction/BXInteractableInterface.h

\- Source/BX/Private/Interaction/BXInteractableInterface.cpp



UINTERFACE(BlueprintType) で IBXInteractable を定義。



仮想関数:

\- void OnInteract(APlayerCharacterBase\* Interactor)

\- FText GetInteractionPrompt() const(UI 表示用、例「拾う」「開ける」「調べる」)

\- bool CanInteract(APlayerCharacterBase\* Interactor) const(条件チェック)



すべて BlueprintNativeEvent または BlueprintImplementableEvent で BP からも実装可能にする。



\### 2. AC\_BX\_PlayerInteraction の C++ 実装



ファイル:

\- Source/BX/Public/Characters/Components/AC\_BX\_PlayerInteraction.h(既存スタブを置き換え)

\- Source/BX/Private/Characters/Components/AC\_BX\_PlayerInteraction.cpp(新規)



主要メンバ:

\- float InteractionRange = 250.0f(EditDefaultsOnly)

\- FCollisionQueryParams TraceParams

\- TWeakObjectPtr<AActor> CurrentFocusedActor

\- FText CurrentPromptText



主要関数:

\- TickComponent(): 毎フレーム LineTrace → フォーカス対象更新

\- TryInteract(): IA\_BX\_Interact から呼ばれる、現在のフォーカス対象に OnInteract を呼ぶ

\- ScanForInteractable(): カメラから前方に LineTrace、IBXInteractable を実装した Actor を探す



実装方針:

\- LineTrace は Owner キャラのカメラ位置から前方ベクトル方向に飛ばす

\- ヒットしたアクターが IBXInteractable を実装していれば CurrentFocusedActor に設定

\- なければ nullptr クリア



\### 3. APlayerCharacterBase との連携



APlayerCharacterBase.h に既に AC\_BX\_PlayerInteraction\* PlayerInteractionComponent が宣言されているはず(スタブ)。これを CreateDefaultSubobject で実体化する。



Input\_InteractTriggered() の実装:

\- PlayerInteractionComponent->TryInteract() を呼ぶだけ



UE\_LOG 入れて呼ばれているか確認可能にする。



\### 4. テスト用ピックアップ Actor を C++ で用意



ファイル:

\- Source/BX/Public/Items/ABXTestPickupItem.h

\- Source/BX/Private/Items/ABXTestPickupItem.cpp



ACharacter ではなく AActor 派生。

IBXInteractable を実装。



メンバ:

\- UStaticMeshComponent\* MeshComponent(箱メッシュ表示用)

\- FName ItemRowName(EditAnywhere、DT\_BX\_Items の Row Name を指定)

\- int32 Quantity = 1(EditAnywhere)



OnInteract 実装:

\- Interactor->InventoryComponent->TryAddItem(ItemRowName, Quantity) を呼ぶ

\- 成功したら Destroy()

\- UE\_LOG で結果を出力



GetInteractionPrompt 実装:

\- TEXT("拾う") を返す(将来的に DT 参照に変える前提でハードコード)



\### 5. ビルド検証



ビルドが通ることを必ず確認。



\## Sprint 13 完了基準



\- IBXInteractable インターフェースが作成されている

\- UAC\_BX\_PlayerInteraction が実装されている(LineTrace + フォーカス管理 + TryInteract)

\- APlayerCharacterBase 側で PlayerInteractionComponent が CreateDefaultSubobject されている

\- Input\_InteractTriggered() が PlayerInteractionComponent->TryInteract() を呼ぶ

\- ABXTestPickupItem が作成され、IBXInteractable を実装している

\- ビルドが通る(必須)

\- docs/SPRINT\_REPORT\_2026-04-24.md の末尾に Sprint 13 実行結果を追記



\## Sprint 13 でやらないこと



\- BP\_TestPickupItem の Blueprint 派生作成(ユーザーが UE5 でやる)

\- レベルへのテストアイテム配置(ユーザーが UE5 でやる)

\- IA\_BX\_Interact のキーバインド設定(IMC\_BX\_Default に追加、ユーザーが UE5 でやる)

\- ドアや調べる対象の実装(Sprint 14 以降)

\- 派手な UI(プロンプト表示は UE\_LOG だけで十分、後続 Sprint で実装)



\## ユーザー側の作業(C++ 完了後、UE5 エディタで)



1\. IMC\_BX\_Default を開いて、IA\_BX\_Interact を追加(キー: E)

&#x20;  - もし IA\_BX\_Interact アセットがなければ作成(Digital bool)

2\. BP\_TestPickupItem を ABXTestPickupItem から派生作成

3\. BP\_TestPickupItem の Class Defaults で:

&#x20;  - StaticMesh に Engine の Cube などを指定

&#x20;  - ItemRowName に DT\_BX\_Items の存在する RowName を指定(例: ammo\_545x39\_ps)

&#x20;  - Quantity に適当な数(例: 30)

4\. LV\_Test\_Player のフロアに BP\_TestPickupItem を 1 個配置

5\. PIE 起動

6\. プレイヤーで近づいて E 押下 → アイテムが消えてインベントリに追加されればゴール

7\. 出力ログで Pickup 成功のログ確認



\## 報告(Claude Code 用)



完了時に以下を報告:

1\. 作成したファイルパスのリスト

2\. ビルド成功 / 失敗

3\. 既存実装(UAC\_BX\_Inventory)との連携で問題があれば

4\. 次の Sprint(Sprint 14)の推奨内容



最後に commit + push をしてください。

コミットメッセージ: feat: Sprint 13 - AC\_BX\_PlayerInteraction と IBXInteractable インターフェースを実装

