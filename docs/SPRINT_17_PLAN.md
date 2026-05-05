\# Sprint 17 PLAN — 防具貫通計算 + 装備システム(縮小版、armor.csv 新規作成あり)



\## 目的



§14-5-4 〜 §14-5-9 防具貫通計算を実装し、PIE で「弱い武器で防具持ちダミーを撃つ → Blunt ダメージ。強い武器で撃つ → 貫通成功でフルダメージ」が動くところまでを完了基準とする。



\## スコープ



\### Sprint 17 に含める(必須)### スコープ外(後続 Sprint へ)



\- 個別防具アイテム(ベストⅠ、ヘッドギアⅡ など)→ Sprint 18 以降

\- 防具装備の AreaCovered(部位カバー範囲)→ Sprint 18

\- 防具劣化に伴う修理・売却 → Sprint 18

\- プレイヤーが防具を装備する仕組み → Sprint 19

\- AI 敵が防具装備 → Sprint 21



\## 参照



\- docs/SPEC\_PartB.md §14-5-4 実効防具閾値

\- docs/SPEC\_PartB.md §14-5-5 貫通スコア

\- docs/SPEC\_PartB.md §14-5-6 貫通確率

\- docs/SPEC\_PartB.md §14-5-7 貫通成功時ダメージ

\- docs/SPEC\_PartB.md §14-5-8 非貫通時の鈍的ダメージ

\- docs/SPEC\_PartB.md §14-5-9 防具耐久減少

\- DataSource/ammo.csv(Penetration / ArmorDamageRatio フィールド)

\- 既存実装: Source/BX/Public/Data/BXBodyPartHelpers.h

\- 既存実装: Source/BX/Public/Characters/Components/AC\_BX\_WeaponHandler.h

\- 既存実装: Source/BX/Private/Characters/Components/AC\_BX\_WeaponHandler.cpp(PerformFireTrace 改修対象)

\- 既存実装: Source/BX/Public/Items/ABXTestDamageDummy.h(改修対象)



\## 実装内容



\### 1. armor.csv の作成



ファイル: DataSource/armor.csv



内容(SPEC §14-5 サンプル CSV をそのまま):

```csv

Name,ArmorClass,BasePenetrationThreshold,BluntThroughputRatio,DurabilityDamageMultiplier,RepairLossRatio

armor\_class\_1,1,10.0,0.28,1.20,0.08

armor\_class\_2,2,18.0,0.24,1.10,0.10

armor\_class\_3,3,28.0,0.20,1.00,0.12

armor\_class\_4,4,38.0,0.16,0.90,0.15

armor\_class\_5,5,48.0,0.13,0.82,0.18

armor\_class\_6,6,58.0,0.10,0.75,0.22

```



\### 2. FBXArmorClassRow 構造体



ファイル:

\- Source/BX/Public/Data/BXDataStructs.h(または別途 BXArmorTableRow.h)



フィールド:

\- int32 ArmorClass

\- float BasePenetrationThreshold

\- float BluntThroughputRatio

\- float DurabilityDamageMultiplier

\- float RepairLossRatio



USTRUCT(BlueprintType) で BP からも参照可能に。



\### 3. FBXEquippedArmor 構造体(装備中防具インスタンス)



ファイル: Source/BX/Public/Data/BXDataStructs.h(または BXArmorEquipment.h)



フィールド:

\- FName ArmorRowName(DT\_BX\_Armor の RowName 参照、デフォルト "armor\_class\_3")

\- float CurrentDurability(現在耐久度)

\- float MaxDurability(最大耐久度、デフォルト 100.0)



USTRUCT(BlueprintType) で。



\### 4. UAC\_BX\_ArmorEquipment コンポーネント



ファイル:

\- Source/BX/Public/Characters/Components/AC\_BX\_ArmorEquipment.h(新規)

\- Source/BX/Private/Characters/Components/AC\_BX\_ArmorEquipment.cpp(新規)



UCLASS: ClassGroup="BX", BlueprintSpawnableComponent



メンバ:

\- FBXEquippedArmor ChestArmor(EditAnywhere、Sprint 17 では Chest のみ装備可能)

\- TObjectPtr<UDataTable> ArmorDataTable(EditDefaultsOnly)



関数:

\- bool HasArmor(EBXBodyPart Part) const(その部位に防具があるか、Sprint 17 では Chest のみ true)

\- FBXArmorClassRow GetArmorClassRow(EBXBodyPart Part) const

\- float GetCurrentDurability(EBXBodyPart Part) const

\- void ApplyDurabilityDamage(EBXBodyPart Part, float DurabilityDamage)



実装方針:

\- Sprint 17 では Chest 防具のみ実装(他の部位は Sprint 18 で拡張)

\- ArmorDataTable から ChestArmor.ArmorRowName で行を引いて FBXArmorClassRow を取得

\- ApplyDurabilityDamage で CurrentDurability を減算 + clamp(0, MaxDurability)



\### 5. UAC\_BX\_WeaponHandler::PerformFireTrace の改修



既存ファイル:

\- Source/BX/Private/Characters/Components/AC\_BX\_WeaponHandler.cpp



改修内容(SPEC §14-5-4 〜 §14-5-9 通り):



```cpp

// 1. 弾薬と部位を取得(Sprint 16 で実装済み)

const FBXAmmoTableRow\* AmmoRow = ...;

const float BaseDamage = AmmoRow->Damage;

const EBXBodyPart Part = ...;

const float BodyPartMultiplier = FBXBodyPartHelpers::GetBodyPartDamageMultiplier(Part);



// 2. 防具コンポーネント取得

UAC\_BX\_ArmorEquipment\* ArmorComp = HitActor->FindComponentByClass<UAC\_BX\_ArmorEquipment>();



if (ArmorComp \&\& ArmorComp->HasArmor(Part)) {

&#x20;   // 3. 防具データ取得

&#x20;   FBXArmorClassRow ArmorClass = ArmorComp->GetArmorClassRow(Part);

&#x20;   float CurrentDurability = ArmorComp->GetCurrentDurability(Part);

&#x20;   float MaxDurability = 100.0f;  // 仮値



&#x20;   // 4. SPEC §14-5-4 実効閾値

&#x20;   float DurabilityRatio = FMath::Clamp(CurrentDurability / MaxDurability, 0.0f, 1.0f);

&#x20;   float EffectivePenThresh = ArmorClass.BasePenetrationThreshold \* FMath::Lerp(0.55f, 1.0f, DurabilityRatio);



&#x20;   // 5. SPEC §14-5-5 貫通スコア

&#x20;   float PenScore = AmmoRow->Penetration - EffectivePenThresh;



&#x20;   // 6. SPEC §14-5-6 貫通確率

&#x20;   float PenChance;

&#x20;   if (PenScore <= -15.0f) PenChance = 0.0f;

&#x20;   else if (PenScore >= 15.0f) PenChance = 1.0f;

&#x20;   else PenChance = 0.5f + (PenScore / 30.0f);



&#x20;   // 7. ランダムロール

&#x20;   bool bPenetrated = FMath::FRand() < PenChance;



&#x20;   float FinalDamage;

&#x20;   if (bPenetrated) {

&#x20;       // SPEC §14-5-7 貫通成功時

&#x20;       float Falloff = FMath::Clamp(1.0f - FMath::Max(0.0f, EffectivePenThresh - AmmoRow->Penetration) \* 0.01f, 0.70f, 1.00f);

&#x20;       FinalDamage = BaseDamage \* BodyPartMultiplier \* Falloff;

&#x20;       UE\_LOG(LogTemp, Warning, TEXT("Penetrated! PenScore=%.2f Chance=%.2f Falloff=%.2f"), PenScore, PenChance, Falloff);

&#x20;   } else {

&#x20;       // SPEC §14-5-8 非貫通(Blunt)

&#x20;       FinalDamage = BaseDamage \* BodyPartMultiplier \* ArmorClass.BluntThroughputRatio;

&#x20;       UE\_LOG(LogTemp, Warning, TEXT("Blunt! PenScore=%.2f Chance=%.2f BluntRatio=%.2f"), PenScore, PenChance, ArmorClass.BluntThroughputRatio);

&#x20;   }



&#x20;   // 8. SPEC §14-5-9 防具耐久減少

&#x20;   float DurabilityDmg = BaseDamage \* AmmoRow->ArmorDamageRatio \* ArmorClass.DurabilityDamageMultiplier;

&#x20;   ArmorComp->ApplyDurabilityDamage(Part, DurabilityDmg);



&#x20;   // 9. 部位ダメージ適用

&#x20;   HealthBodyParts->ApplyDamageToBodyPart(Part, FinalDamage);

} else {

&#x20;   // 防具なし(Sprint 16 のロジック)

&#x20;   float FinalDamage = BaseDamage \* BodyPartMultiplier;

&#x20;   HealthBodyParts->ApplyDamageToBodyPart(Part, FinalDamage);

}

```



ログ出力:
### 6. ABXTestDamageDummy に ArmorEquipment コンポーネント追加



ファイル:

\- Source/BX/Public/Items/ABXTestDamageDummy.h

\- Source/BX/Private/Items/ABXTestDamageDummy.cpp



メンバ追加:

\- UAC\_BX\_ArmorEquipment\* ArmorComponent



CreateDefaultSubobject で生成、デフォルト ArmorClass=3 を装備。



\### 7. ビルド検証



ビルドが通ることを必ず確認。



\## Sprint 17 完了基準



\- DataSource/armor.csv が作成されている(6 行のクラス情報)

\- FBXArmorClassRow / FBXEquippedArmor 構造体が実装されている

\- UAC\_BX\_ArmorEquipment コンポーネントが実装されている

\- PerformFireTrace が SPEC §14-5-4 〜 §14-5-9 の計算式を実装している

\- ABXTestDamageDummy に ArmorComponent が追加されている

\- ビルドが通る(必須)

\- docs/SPRINT\_REPORT\_2026-04-24.md に Sprint 17 実行結果を追記



\## ユーザー側の作業(C++ 完了後、UE5 エディタで)



1\. DT\_BX\_Armor を作成: /Content/BX/Data/Common/Armor/ → Data Table → Row Structure: FBXArmorClassRow → 名前: DT\_BX\_Armor → ammor.csv からインポート

2\. BP\_BX\_Player の WeaponHandlerComponent > ArmorDataTable は不要(プレイヤー用ではないため)

3\. BP\_TestDamageDummy の ArmorComponent > ArmorDataTable に DT\_BX\_Armor をアサイン(設定する場合)

4\. BP\_TestDamageDummy の ArmorComponent > ChestArmor > ArmorRowName を "armor\_class\_3" にデフォルトセット

5\. BP\_TestDamageDummy の ArmorComponent > ChestArmor > MaxDurability = 100, CurrentDurability = 100

6\. PIE 起動 → 武器ピックアップ拾う → ダミーを撃つ

7\. 出力ログ確認:

&#x20;  - 9mm FMJ(Pen=12) → 「Blunt!」ログ + Damage 6.8pt 程度

&#x20;  - 5.56 FMJ(Pen=32) → 確率 63% で「Penetrated!」ログ + Damage 44pt

&#x20;  - 連射するうちに ArmorDurability が減って、貫通確率上昇



\## 報告(Claude Code 用)



完了時に以下を報告:

1\. 作成 / 更新したファイルパスのリスト(armor.csv 含む)

2\. 追加した関数 / 構造体 / コンポーネントのリスト

3\. ビルド成功 / 失敗(失敗時はエラーメッセージ)

4\. 実装した計算式の SPEC 対応(§14-5-4 〜 §14-5-9 すべて反映されているか)

5\. ログ出力例(Penetrated と Blunt の両ケース)

6\. ユーザーが UE5 エディタでやるべき作業(箇条書き)

7\. 次の Sprint(Sprint 18)の推奨内容



最後に commit + push をしてください。

コミットメッセージ: feat: Sprint 17 - 防具貫通計算実装(armor.csv + FBXArmorClassRow + UAC\_BX\_ArmorEquipment + SPEC §14-5-4〜§14-5-9 計算式)

