\# Sprint 14 PLAN — AC\_BX\_WeaponHandler 実装



\## 目的



§14-4 武器 DataTable と §14-5 ダメージ計算モデルに基づき、プレイヤーが武器を装備して発砲できる仕組み `UAC\_BX\_WeaponHandler` を C++ で実装する。



PIE で「左クリック → LineTrace で発砲ログ + 弾薬消費 + 命中アクターへのダメージ通知」が動くところまでを完了基準とする。



ただし以下は今回スコープ外:

\- VFX(マズルフラッシュ、銃口炎、弾道トレーサー)

\- SE(発砲音、リロード音)

\- 反動 / 拡散

\- AI 敵への着弾(Sprint 15 以降)



\## 参照



\- docs/SPEC\_PartB.md §14-4 武器 DataTable の行構造詳細

\- docs/SPEC\_PartB.md §14-5 ダメージ計算モデル(まずは LineTrace + Hit 通知だけ、貫通計算は Sprint 15)

\- docs/SPEC\_PartB.md §23-5 武器保持コンポーネント(あれば)

\- docs/SPEC\_PartB.md §15-1 アイテム DataTable とインベントリ基本構造

\- 既存実装: Source/BX/Public/Items/Components/UAC\_BX\_Inventory.h

\- 既存スタブ: Source/BX/Public/Characters/Components/AC\_BX\_WeaponHandler.h

\- 既存実装: Source/BX/Public/Characters/APlayerCharacterBase.h

\- 既存実装: Source/BX/Public/Data/BXEnums.h(EBXWeaponSlot, EBXCombatState は既存)



\## Sprint 14 スコープ



\### 1. UAC\_BX\_WeaponHandler の C++ 実装



ファイル:

\- Source/BX/Public/Characters/Components/AC\_BX\_WeaponHandler.h(既存スタブを置き換え)

\- Source/BX/Private/Characters/Components/AC\_BX\_WeaponHandler.cpp(新規)



主要メンバ(UPROPERTY):

\- EBXWeaponSlot CurrentSlot(現在装備中スロット、デフォルト Primary)

\- TMap<EBXWeaponSlot, FName> EquippedWeaponRowNames(各スロットの武器の RowName、DT\_BX\_Weapons 参照用)

\- TMap<EBXWeaponSlot, int32> CurrentMagazineAmmo(各スロットの装弾数)

\- bool bIsReloading

\- float FireCooldownRemaining(連射制御)

\- TObjectPtr<UDataTable> WeaponDataTable(EditDefaultsOnly、DT\_BX\_Weapons をアサインする)



主要関数:

\- TickComponent(): FireCooldownRemaining を減算

\- EquipWeapon(EBXWeaponSlot Slot, FName WeaponRowName): スロットに武器をセット、装弾数を初期化

\- SwitchWeaponSlot(EBXWeaponSlot NewSlot): 現在スロット切り替え

\- FirePrimary(): 現在装備中武器で 1 発撃つ。クールダウン・装弾数・LineTrace を処理

\- Reload(): 装弾数をマガジン上限まで戻す(InventoryComponent から弾薬消費は今回ハードコード OK、本実装は Sprint 15)

\- GetCurrentWeaponRow(): 現在装備中武器の DT 行を取得



実装方針:

\- LineTrace はキャラのカメラ位置から前方ベクトル方向に飛ばす

\- TraceDistance は Default 5000.0f(50m)

\- ヒットしたら UE\_LOG で命中物を出力

\- ヒット先が UPrimitiveComponent ならば仮ダメージ通知(UGameplayStatics::ApplyPointDamage を呼ぶ)

\- 詳細な貫通計算 / 部位判定 / 防具計算は Sprint 15 以降



\### 2. APlayerCharacterBase との連携



APlayerCharacterBase の WeaponHandlerComponent を CreateDefaultSubobject で実体化(Sprint 11 でスタブ宣言だけしてた箇所を実体化)。



Input\_FirePrimaryStarted() の実装:

\- WeaponHandlerComponent->FirePrimary() を呼ぶ



Input\_ReloadTriggered() の実装:

\- WeaponHandlerComponent->Reload() を呼ぶ



Input\_SwitchWeaponPrimary() / SwitchWeaponSecondary() / SwitchWeaponPistol() / SwitchWeaponMelee() の実装:

\- WeaponHandlerComponent->SwitchWeaponSlot(対応するスロット) を呼ぶ



各関数の冒頭に UE\_LOG を入れて呼ばれているか確認可能にする。



\### 3. テスト用武器ピックアップ Actor



ファイル:

\- Source/BX/Public/Items/ABXTestWeaponPickup.h

\- Source/BX/Private/Items/ABXTestWeaponPickup.cpp



ABXTestPickupItem(Sprint 13)と似た構造。AActor 派生、IBXInteractable 実装。



メンバ:

\- UStaticMeshComponent\* MeshComponent

\- FName WeaponRowName(EditAnywhere、DT\_BX\_Weapons の Row Name を指定)

\- EBXWeaponSlot TargetSlot(EditAnywhere、デフォルト Primary)



OnInteract 実装:

\- Interactor->WeaponHandlerComponent->EquipWeapon(TargetSlot, WeaponRowName) を呼ぶ

\- Destroy()

\- UE\_LOG で結果出力



GetInteractionPrompt 実装:

\- TEXT("武器を拾う") を返す



\### 4. ビルド検証



ビルドが通ることを必ず確認。



\## Sprint 14 完了基準



\- UAC\_BX\_WeaponHandler が実装されている(EquipWeapon, FirePrimary, Reload, SwitchWeaponSlot)

\- APlayerCharacterBase 側で WeaponHandlerComponent が CreateDefaultSubobject されている

\- Input\_FirePrimaryStarted / Input\_ReloadTriggered / Input\_SwitchWeaponSlot 系が実装されている

\- ABXTestWeaponPickup が実装され、IBXInteractable で武器を装備できる

\- ビルドが通る(必須)

\- docs/SPRINT\_REPORT\_2026-04-24.md の末尾に Sprint 14 実行結果を追記



\## Sprint 14 でやらないこと



\- VFX / SE

\- 反動 / 拡散

\- 弾道(プロジェクタイルではなく LineTrace のみ)

\- 詳細な貫通計算 / 部位ダメージ(Sprint 15)

\- AI 敵への着弾検出(Sprint 15)

\- HUD 表示(Sprint 16)

\- Inventory との詳細な弾薬消費連携(マガジン上限まで戻すハードコードで OK)



\## ユーザー側の作業(C++ 完了後、UE5 エディタで)



1\. IMC\_BX\_Default に以下を追加(IA がなければ作成):

&#x20;  - IA\_BX\_FirePrimary(マウス左ボタン、Digital bool)

&#x20;  - IA\_BX\_Reload(R キー、Digital bool)

&#x20;  - IA\_BX\_SwitchWeaponPrimary(1 キー、Digital bool)

&#x20;  - IA\_BX\_SwitchWeaponSecondary(2 キー、Digital bool)

&#x20;  - IA\_BX\_SwitchWeaponPistol(3 キー、Digital bool)

&#x20;  - IA\_BX\_SwitchWeaponMelee(4 キー、Digital bool)

2\. BP\_BX\_Player の Class Defaults で上記 IA をアサイン

3\. BP\_BX\_Player の Class Defaults で WeaponDataTable に DT\_BX\_Weapons をアサイン(WeaponHandlerComponent のプロパティ)

4\. BP\_TestWeaponPickup を ABXTestWeaponPickup から派生作成

&#x20;  - StaticMesh: 武器っぽい何か(立方体 + 細長スケールで OK)

&#x20;  - WeaponRowName: DT\_BX\_Weapons に存在する Row Name(例: weapon\_ak74m\_01 等、要確認)

&#x20;  - TargetSlot: Primary

5\. LV\_Test\_Player に BP\_TestWeaponPickup を配置

6\. PIE 起動

7\. プレイヤーで近づいて E 押下 → 装備

8\. 左クリックで発砲ログ出力確認

9\. R でリロード確認

10\. 1〜4 キーでスロット切り替え確認



\## 報告(Claude Code 用)



完了時に以下を報告:

1\. 作成 / 更新したファイルパスのリスト

2\. 追加した関数のリスト

3\. ビルド成功 / 失敗(失敗時はエラーメッセージ)

4\. 既存実装(UAC\_BX\_Inventory, APlayerCharacterBase 等)との連携で問題があれば

5\. ユーザーが UE5 エディタでやるべき作業(箇条書き)

6\. 次の Sprint(Sprint 15)の推奨内容



最後に commit + push をしてください。

コミットメッセージ: feat: Sprint 14 - AC\_BX\_WeaponHandler 実装(発砲・リロード・スロット切替)

