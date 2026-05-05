# Sprint 16 PLAN — 弾薬種類別ダメージ + 部位マルチプライヤー(防具実装は Sprint 17)

## 目的

§14-5 ダメージ計算モデルの**前半部分**を実装:
- 弾薬種類別の Damage / Penetration を使う(現状ハードコード 44 を排除)
- 部位ダメージマルチプライヤーを適用(Head x4.0, Chest x1.0 など)
- 武器→弾薬連携(WeaponRow.AmmoRowName または Caliber → AmmoRow)

PIE で「Head を撃つと 1 発で死ぬ、Leg は何発か必要」が動くところまでを完了基準とする。

## スコープ外(Sprint 17 へ繰り延べ)

- 防具貫通計算(armor.csv が未作成)
- ArmorClass / AreaCovered ロジック
- ArmorDamageRatio による防具劣化
- FragmentChance による破片効果
- VelocityFactor による距離減衰

## 参照

- docs/SPEC_PartB.md §14-5 ダメージ計算モデル(部位マルチプライヤーの数値確認)
- docs/SPEC_PartB.md §14-4 武器 DataTable(AmmoRowName または Caliber 参照方法)
- docs/SPEC_PartB.md §14-2 弾薬テーブル(構造確認)
- DataSource/ammo.csv(構造: Name, AmmoId, DisplayName, Caliber, Penetration, Damage, ArmorDamageRatio, FragmentChance, VelocityFactor)
- DataSource/weapons.csv(武器が弾薬を参照する仕組みを確認)
- 既存実装: Source/BX/Public/Data/BXEnums.h(EBXBodyPart)
- 既存実装: Source/BX/Public/Characters/Components/AC_BX_HealthBodyParts.h
- 既存実装: Source/BX/Public/Characters/Components/AC_BX_WeaponHandler.h
- 既存実装: Source/BX/Private/Characters/Components/AC_BX_WeaponHandler.cpp(FirePrimary 改修対象)

## Sprint 16 スコープ

### 1. FBXAmmoDataRow 構造体の確認 / 整備

ファイル:
- Source/BX/Public/Data/BXDataStructs.h(既存)

ammo.csv に対応する FBXAmmoDataRow が既にあるかチェック:
- フィールド: Name, AmmoId, DisplayName, Caliber, Penetration, Damage, ArmorDamageRatio, FragmentChance, VelocityFactor

なければ追加。既存ならフィールド名が CSV と一致するか確認。

### 2. 部位ダメージマルチプライヤー定義

ファイル:
- Source/BX/Public/Data/BXBodyPartHelpers.h(Sprint 15 で作成済み)
- Source/BX/Private/Data/BXBodyPartHelpers.cpp

関数追加:
- static float GetBodyPartDamageMultiplier(EBXBodyPart Part)

数値(SPEC §14-5 通り、なければ仮値):
- Head: 4.0(致命傷部位)
- Chest: 1.0(基準)
- Abdomen: 1.5
- LeftArm / RightArm: 0.65
- LeftLeg / RightLeg: 0.7

(SPEC で定義されている数値があればそれに従う、なければ上記の仮値で進める)

### 3. AC_BX_WeaponHandler::PerformFireTrace の改修

既存ファイル:
- Source/BX/Private/Characters/Components/AC_BX_WeaponHandler.cpp

改修内容:
- 武器の現在装備武器 Row から AmmoRowName(または Caliber)を取得
- AmmoDataTable から該当 Ammo Row を引く(AmmoDataTable は新規 UPROPERTY 追加、EditDefaultsOnly)
- BaseDamage = AmmoRow.Damage(現状ハードコード 44 を置き換え)
- 部位判定後、Multiplier = GetBodyPartDamageMultiplier(Part) を取得
- FinalDamage = BaseDamage * Multiplier
- ApplyDamageToBodyPart(Part, FinalDamage) を呼ぶ
- UE_LOG: "FirePrimary HIT: Part=Head, BaseDmg=44, Mult=4.0, FinalDmg=176"

### 4. AmmoDataTable の追加(BP_BX_Player から)

UAC_BX_WeaponHandler に新規 UPROPERTY:
- TObjectPtr<UDataTable> AmmoDataTable(EditDefaultsOnly、DT_BX_Ammo をアサインする)

ユーザーが UE5 エディタで BP_BX_Player の Class Defaults でアサイン可能にする。

### 5. 武器→弾薬連携の方法決定

weapons.csv を確認して、武器が弾薬を参照する方法を選ぶ:

- 案A: 武器 Row に AmmoRowName フィールドがある → 直接 RowName で AmmoDataTable を引く
- 案B: 武器 Row に Caliber フィールドがある → Caliber で AmmoDataTable を線形検索
- 案C: 別途 WeaponAmmoMappingTable がある → 仕様確認

実装は weapons.csv の実際の構造に合わせる。

### 6. ビルド検証

ビルドが通ることを必ず確認。

## Sprint 16 完了基準

- FBXAmmoDataRow が ammo.csv と一致している
- GetBodyPartDamageMultiplier が実装されている
- PerformFireTrace が AmmoRow.Damage と部位 Multiplier を使う
- AmmoDataTable が UAC_BX_WeaponHandler の UPROPERTY に追加されている
- ビルドが通る(必須)
- docs/SPRINT_REPORT_2026-04-24.md に Sprint 16 実行結果を追記

## ユーザー側の作業(C++ 完了後、UE5 エディタで)

1. DT_BX_Ammo を Reimport(まだなら)
2. BP_BX_Player の Class Defaults で WeaponHandlerComponent > AmmoDataTable に DT_BX_Ammo をアサイン
3. PIE 起動 → ダミーを撃つ
4. ダミーの Head を狙って撃つ → 1〜2 発で IsDead=true(Mult=4.0 で 44*4=176 ダメージ、Head HP=35 を一撃)
5. ダミーの Chest を狙う → 3 発で死ぬ(従来通り 44pt)
6. ダミーの Leg を狙う → 死なない(Leg=65 vs 44*0.7=30.8、HP=65 で何発か必要)

## 報告(Claude Code 用)

完了時に以下を報告:
1. 作成 / 更新したファイルパスのリスト
2. 追加した関数 / 構造体のリスト
3. ビルド成功 / 失敗(失敗時はエラーメッセージ)
4. 武器→弾薬連携の方式(案A/B/C どれを採用したか)
5. SPEC §14-5 の部位マルチプライヤー数値(仮値で実装した場合は明記)
6. ユーザーが UE5 エディタでやるべき作業(箇条書き)
7. 次の Sprint(Sprint 17)の推奨内容

最後に commit + push をしてください。
コミットメッセージ: feat: Sprint 16 - 弾薬種類別ダメージ + 部位マルチプライヤー実装(防具は Sprint 17)