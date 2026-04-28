\# Sprint 15 PLAN — 部位ダメージシステム(基盤実装、スコープ縮小版)



\## 目的



§14-3 部位ダメージの基盤として、UAC\_BX\_HealthBodyParts コンポーネントを実装し、PIE で「ダメージダミー Actor を撃つ → 部位別 HP が減るログ出力」が動くところまでを完了基準とする。



ただし以下は今回スコープ外:

\- 防具貫通計算(Sprint 16)

\- 弾薬種類別ダメージ計算(Sprint 16)

\- UI 表示(Sprint 17 HUD で実装)

\- 死亡判定 / 復活処理(Sprint 18)

\- プレイヤーキャラへの部位ダメージ適用(Sprint 19)

\- ボーン階層に依存した精密な部位判定(今回は BoneName からの簡易マッピングのみ)



\## 参照



\- docs/SPEC\_PartB.md §14-3 部位ダメージ

\- docs/SPEC\_PartB.md §14-5 ダメージ計算モデル(参考のみ、フル実装は Sprint 16)

\- 既存 enum: Source/BX/Public/Data/BXEnums.h(EBXBodyPart は既存のはず)

\- 既存スタブ: Source/BX/Public/Characters/Components/AC\_BX\_HealthBodyParts.h

\- 既存実装: Source/BX/Public/Characters/Components/AC\_BX\_WeaponHandler.h(Sprint 14 で実装済み)

\- 既存実装: Source/BX/Private/Characters/Components/AC\_BX\_WeaponHandler.cpp(FirePrimary を改修)

\- docs/SPRINT\_REPORT\_2026-04-24.md(Sprint 1-14 実装履歴)



\## Sprint 15 スコープ



\### 1. UAC\_BX\_HealthBodyParts の C++ 実装



ファイル:

\- Source/BX/Public/Characters/Components/AC\_BX\_HealthBodyParts.h(既存スタブを置き換え)

\- Source/BX/Private/Characters/Components/AC\_BX\_HealthBodyParts.cpp(新規)



主要メンバ(UPROPERTY):

\- TMap<EBXBodyPart, float> BodyPartHP(各部位の現在 HP)

\- TMap<EBXBodyPart, float> BodyPartMaxHP(各部位の最大 HP、デフォルト値表)

\- bool bIsDead



主要関数:

\- BeginPlay(): 7部位の HP を初期化

&#x20; - Head: 35

&#x20; - Chest: 100  ← Chest 表記(SPEC §14-3 通り)

&#x20; - Abdomen: 70

&#x20; - LeftArm: 60

&#x20; - RightArm: 60

&#x20; - LeftLeg: 65

&#x20; - RightLeg: 65

\- ApplyDamageToBodyPart(EBXBodyPart Part, float Damage): 部位 HP を減らす、UE\_LOG 出力

\- GetBodyPartHP(EBXBodyPart Part): 現在 HP 取得

\- GetBodyPartHPRatio(EBXBodyPart Part): HP 割合(0.0〜1.0)取得

\- IsDead(): bIsDead を返す(Head=0 または Chest=0 で死亡判定、ただし今回は判定するだけで死亡処理なし)



実装方針:

\- 各 ApplyDamageToBodyPart で UE\_LOG: "Part=Head HP=35→13 (-22)"のような分かりやすい出力

\- HP が負にならないように clamp(0)

\- 死亡判定だけは入れる(bIsDead = true、ログのみ出力)



\### 2. BoneName → EBXBodyPart マッピング関数



ファイル:

\- Source/BX/Public/Data/BXBodyPartHelpers.h(新規、static 関数集)

\- Source/BX/Private/Data/BXBodyPartHelpers.cpp(新規)



関数:

\- static EBXBodyPart BoneNameToBodyPart(FName BoneName)



マッピング規則(UE5 SK\_Mannequin 標準ボーン名 + 簡易対応):

\- "head", "neck\_01" → Head

\- "spine\_01", "spine\_02", "spine\_03", "pelvis" → Chest または Abdomen(spine\_03 上半身は Chest、spine\_01 下半身は Abdomen、pelvis は Abdomen)

\- "upperarm\_l", "lowerarm\_l", "hand\_l" など Left 系 → LeftArm

\- "upperarm\_r", "lowerarm\_r", "hand\_r" など Right 系 → RightArm

\- "thigh\_l", "calf\_l", "foot\_l" など Left 系 → LeftLeg

\- "thigh\_r", "calf\_r", "foot\_r" など Right 系 → RightLeg

\- 不明なボーン名 → Chest(デフォルト)



実装方針:

\- FName を FString に変換 → ToLower で大文字小文字無視 → Contains で部分一致

\- Switch + if-else でハードコード OK

\- UE\_LOG で「BoneName=upperarm\_l → BodyPart=LeftArm」のようなマッピング結果を出力



\### 3. AC\_BX\_WeaponHandler::FirePrimary の改修



既存ファイル:

\- Source/BX/Private/Characters/Components/AC\_BX\_WeaponHandler.cpp



改修内容:

\- LineTrace の結果(FHitResult)から HitResult.BoneName を取得

\- HitResult.GetActor() に UAC\_BX\_HealthBodyParts コンポーネントがあるかチェック

\- あれば:

&#x20; - BXBodyPartHelpers::BoneNameToBodyPart(BoneName) で部位を決定

&#x20; - HealthBodyParts->ApplyDamageToBodyPart(Part, BaseDamage) を呼ぶ

\- UE\_LOG で「FirePrimary HIT: Actor=X, Bone=Y, Part=Z, Dmg=W」



LineTrace の改修:

\- FCollisionQueryParams で bReturnPhysicalMaterial = true

\- bTraceComplex = true(複雑なコリジョンでボーンを取得)

\- ECollisionChannel は ECC\_Visibility や Pawn など適切なチャネルを選択



\### 4. テスト用ダメージダミー Actor



ファイル:

\- Source/BX/Public/Items/ABXTestDamageDummy.h

\- Source/BX/Private/Items/ABXTestDamageDummy.cpp



AActor 派生(ACharacter ではない、シンプルに保つ)。



メンバ:

\- USkeletalMeshComponent\* SkeletalMesh(SK\_Mannequin を ClassDefault でアサイン可能、ない場合は UStaticMeshComponent でも可)

\- UAC\_BX\_HealthBodyParts\* HealthComponent(CreateDefaultSubobject)



実装:

\- BeginPlay で UE\_LOG「ダミー作成」

\- HealthComponent のログがあれば、撃った時の状態が分かる



注:SkeletalMesh をセットせず、まずは StaticMesh(Cube)でも OK。

&#x20;  ただし StaticMesh だとボーン名が取得できないので、すべて Chest にマップされる。

&#x20;  それでも「ダメージが届く」ことの確認はできる。



\### 5. ビルド検証



ビルドが通ることを必ず確認。



\## Sprint 15 完了基準



\- UAC\_BX\_HealthBodyParts が実装されている(7部位 HP 管理 + ApplyDamageToBodyPart)

\- BXBodyPartHelpers の BoneNameToBodyPart が実装されている

\- AC\_BX\_WeaponHandler::FirePrimary が改修されている(BoneName → 部位ダメージ通知)

\- ABXTestDamageDummy が実装されている(IBXInteractable 不要、ダメージ受け取り役)

\- ビルドが通る(必須)

\- docs/SPRINT\_REPORT\_2026-04-24.md の末尾に Sprint 15 実行結果を追記



\## Sprint 15 でやらないこと



\- 防具貫通計算(Sprint 16)

\- 弾薬種類別ダメージ計算(Sprint 16)

\- UI 表示(Sprint 17 HUD)

\- 死亡時の処理(リスポーン、Ragdoll など、Sprint 18)

\- プレイヤーキャラへの部位ダメージ適用(Sprint 19)

\- 状態異常との連携(出血、骨折)(Sprint 20 状態異常で実装)

\- 動的なボーン階層対応(複雑なキャラに対応するのは将来)

\- AI 敵への適用(Sprint 21+)



\## ユーザー側の作業(C++ 完了後、UE5 エディタで)



1\. BP\_TestDamageDummy を ABXTestDamageDummy から派生作成

&#x20;  - Content/BX/Blueprints/Items/ に作成

&#x20;  - SkeletalMesh があるなら SK\_Mannequin をアサイン(なければ StaticMesh で Cube)

2\. LV\_Test\_Player にダミーを配置(プレイヤースタートから 5〜10m 先に)

3\. PIE 起動

4\. プレイヤーで武器ピックアップを E で拾う(Sprint 13 + 14 の動作)

5\. ダミーに向かって左クリックで発砲

6\. 出力ログで「FirePrimary HIT: Actor=BP\_TestDamageDummy\_C\_0, Bone=spine\_03, Part=Chest, Dmg=44」のような表示を確認

7\. 連射してダミーの各部位 HP を 0 に近づけ、最終的に IsDead=true ログが出ればゴール



\## 報告(Claude Code 用)



完了時に以下を報告:

1\. 作成 / 更新したファイルパスのリスト

2\. 追加した関数のリスト

3\. ビルド成功 / 失敗(失敗時はエラーメッセージ)

4\. EBXBodyPart の enum 値の確認(Chest 表記を使用しているか)

5\. SK\_Mannequin 不在の場合の代替案(StaticMesh + デフォルト Chest マッピング)

6\. ユーザーが UE5 エディタでやるべき作業(箇条書き)

7\. 次の Sprint(Sprint 16)の推奨内容(防具貫通計算 + 弾薬種類別)



最後に commit + push をしてください。

コミットメッセージ: feat: Sprint 15 - 部位ダメージシステム基盤(HealthBodyParts + BoneName マッピング + FirePrimary 改修)

