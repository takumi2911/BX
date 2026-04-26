\# Sprint 11 PLAN: APlayerCharacterBase の C++ 実装



\## 目的



§23-4 プレイヤー基底クラス `APlayerCharacterBase` を C++ で実装し、

PIE で歩き回れる状態にする。BX プロジェクトの「歩いて撃てる」状態への第一歩。



\## 参照ドキュメント(Claude Code が必ず読むこと)



1\. `docs/SPEC\_PartB.md` の §23-4(プレイヤー基底クラス)

2\. `docs/SPEC\_PartB.md` の §16-4(SaveGame 参照構造体詳細)

3\. `docs/SPRINT\_REPORT\_2026-04-24.md`(Sprint 1-10 実装履歴)

4\. 既存実装: `Source/BX/Public/Items/Components/UAC\_BX\_Inventory.h`

5\. 既存 enum: `Source/BX/Public/Common/BXEnums.h`(または相当ファイル)



\## Claude Code への指示文(明日朝、以下をそのままコピペ)



\---



Sprint 11 を開始してください。



\# Sprint 11 目的

§23-4 プレイヤー基底クラス `APlayerCharacterBase` を C++ で実装し、PIE で歩き回れる状態にする。



\# 参照ドキュメント



必ず以下を最初に読んでください:



1\. `docs/SPEC\_PartB.md` の §23-4(プレイヤー基底クラス)

2\. `docs/SPEC\_PartB.md` の §16-4(SaveGame 参照構造体詳細)

3\. `docs/SPRINT\_REPORT\_2026-04-24.md`(Sprint 1-10 実装履歴)

4\. 既存実装: `Source/BX/Public/Items/Components/UAC\_BX\_Inventory.h`

5\. 既存 enum: `Source/BX/Public/Common/BXEnums.h`



\# Sprint 11 スコープ



\## 1. 必要な新規 enum を `BXEnums.h` に追加



```cpp
## 2. APlayerCharacterBase クラス作成

ファイル:
- `Source/BX/Public/Characters/APlayerCharacterBase.h`
- `Source/BX/Private/Characters/APlayerCharacterBase.cpp`

クラス継承: `ACharacter` を継承。

必須コンポーネント(初期化):
- `Mesh1P` (USkeletalMeshComponent*) - 一人称メッシュ
- `CameraFirstPerson` (UCameraComponent*) - 一人称カメラ
- `SpringArm` (USpringArmComponent*) - 三人称用アーム
- `CameraThirdPerson` (UCameraComponent*) - 三人称カメラ
- `InventoryComponent` (UAC_BX_Inventory*) - 既存実装
- `StatusEffectsComponent` (UAC_BX_StatusEffects*) - 既存なら有効化、未実装なら宣言のみ
- `HealthBodyPartsComponent` (UAC_BX_HealthBodyParts*) - 既存なら有効化、未実装なら宣言のみ
- `WeaponHandlerComponent` (UAC_BX_WeaponHandler*) - 未実装、宣言のみ(Sprint 14 で実装)
- `PlayerInteractionComponent` (UAC_BX_PlayerInteraction*) - 未実装、宣言のみ(Sprint 13 で実装)

注意:
- 未実装コンポーネントは TODO コメント付きで CreateDefaultSubobject を省略
- 既存実装の確認は Source/BX/Public/Characters/Components/ 配下で行う

## 3. Enhanced Input の入力アクション宣言(24個)

ヘッダに `TObjectPtr<UInputAction>` で宣言(アセットの作成は明日以降):

IA_BX_Move, IA_BX_Look, IA_BX_Jump, IA_BX_Sprint
IA_BX_Crouch, IA_BX_Prone, IA_BX_LeanLeft, IA_BX_LeanRight
IA_BX_FirePrimary, IA_BX_FireSecondary, IA_BX_Reload
IA_BX_SwitchWeaponPrimary, IA_BX_SwitchWeaponSecondary
IA_BX_SwitchWeaponPistol, IA_BX_SwitchWeaponMelee, IA_BX_Holster
IA_BX_Interact, IA_BX_MedicalUse
IA_BX_QuickSlot1, IA_BX_QuickSlot2, IA_BX_QuickSlot3, IA_BX_QuickSlot4
IA_BX_OpenInventory, IA_BX_OpenMap

各 Input_*関数のスタブ実装(空の関数 + UE_LOG)も宣言。

## 4. 視点・姿勢・移動・戦闘状態の保持

```cpp
初期値: bAlive=true、その他 false / 各 enum の最初の値。

## 5. HUD 公開フィールド(最低限)

- HudCarryWeightKg (float)
- HudCarryMaxWeightKg (float)
- HudCarryState (EBXCarryState)
- HudCurrentLanguage (EBXLanguage)

## 6. 主要メンバ関数の宣言とスタブ実装

§23-4 の関数シグネチャに従って:
- InitializePlayerCharacter()
- BindAllInputActions(UEnhancedInputComponent*)
- SetViewMode(EBXViewMode)
- GetCurrentViewMode() / GetCurrentLocomotionState() / GetCurrentCombatState()
- LockInput(FGameplayTag) / UnlockInput(FGameplayTag) / IsInputLocked()
- NotifyDeathOrUnconscious()
- ExportPlayerSaveState() / ImportPlayerSaveState()(SaveGame 関連は構造体未実装ならスタブ)

中身は最低限のスタブ(空実装 + UE_LOG)で OK。詳細は次の Sprint で。

## 7. ビルド検証(必須)

実装後、必ず Build.cs でビルドが通ることを確認。
コンパイルエラーが残ったまま終わらせないでください。

# Sprint 11 完了基準

- [ ] `BXEnums.h` に 7 つの新規 enum が追加されている
- [ ] `APlayerCharacterBase.h` / `.cpp` が作成されている
- [ ] 必須コンポーネントの宣言と CreateDefaultSubobject(実装済みのもの)が書かれている
- [ ] 24 個の入力アクション参照が UPROPERTY で宣言されている
- [ ] 視点・姿勢・移動・戦闘状態の UPROPERTY が宣言されている
- [ ] 主要関数のスタブが定義されている
- [ ] **ビルドが通る**(必須)
- [ ] `docs/SPRINT_REPORT_2026-04-24.md` の末尾に Sprint 11 実行結果を追記

# Sprint 11 でやらないこと

- BP_BX_Player の Blueprint 派生作成(明日以降エディタ手作業)
- AC_BX_WeaponHandler、AC_BX_PlayerInteraction 等の未実装コンポーネントの実装(別 Sprint)
- 入力アクション IA_BX_* のアセット作成(明日以降エディタ作業)
- ANIM ブループリントの設定
- BP_BX_Player をデフォルト Pawn に設定する作業

# 報告

Sprint 11 完了時に以下を報告:
1. 作成したファイルパスのリスト
2. 追加した enum のリスト
3. ビルド成功 / 失敗(失敗時はエラーメッセージ)
4. 既存実装(AC_BX_Inventory 等)との連携で問題があれば
5. 次の Sprint(Sprint 12)の推奨内容

最後に commit + push:
コミットメッセージ:
`feat: Sprint 11 - APlayerCharacterBase 基底クラスと関連 enum を実装`

---