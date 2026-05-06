// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/BXEnums.h"

class APlayerCharacterBase;

#include "BXHUDWidget.generated.h"

// HUD ウィジェット基底クラス (SPEC §16)
// C++ から BlueprintImplementableEvent で BP 側に HP / 弾薬を通知する。
// WBP_BX_HUD はこのクラスを親クラスとして UE5 エディタで作成する。
UCLASS(BlueprintType, Blueprintable)
class BX_API UBXHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // オーナープレイヤーを設定する (BeginPlay 直後に呼ぶ)
    // ※ UUserWidget::SetOwningPlayer との名前衝突を避けるため InitializeForPlayer を使用
    UFUNCTION(BlueprintCallable, Category="BX|HUD")
    void InitializeForPlayer(APlayerCharacterBase* Player);

    // 部位 HP 比率を通知 (0.0〜1.0)。BP 側でプログレスバーを更新する
    UFUNCTION(BlueprintImplementableEvent, Category="BX|HUD")
    void OnUpdateBodyPartHP(EBXBodyPart Part, float HPRatio);

    // 弾薬数を通知。BP 側でテキストを更新する
    UFUNCTION(BlueprintImplementableEvent, Category="BX|HUD")
    void OnUpdateAmmo(int32 Current, int32 MagSize);

    // 現在装備中武器の RowName を通知
    UFUNCTION(BlueprintImplementableEvent, Category="BX|HUD")
    void OnUpdateCurrentWeapon(FName WeaponRowName);

protected:
    UPROPERTY(BlueprintReadOnly, Category="BX|HUD")
    TWeakObjectPtr<APlayerCharacterBase> OwningPlayer;
};
