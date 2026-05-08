// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"

#include "ABXNavigationLink.generated.h"

UENUM(BlueprintType)
enum class EBXNavLinkType : uint8
{
    Jump,      // ジャンプで越える (敵が跳べるとき)
    Drop,      // 落下のみ (一方通行)
    Climb,     // 障害物をよじ登る
    Door,      // ドア通過 (アニメーション付き)
};

// BX カスタムナビゲーションリンク (Sprint 28)
// NavLinkProxy を継承し、AI が特定の動作で移動できるリンクを定義する。
// 工場の梯子・跳び越えられる低い壁・落下ポイント等に配置。
// ABXEnemyAIController が NavMesh を使う際にこのリンクを経由できる。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXNavigationLink : public ANavLinkProxy
{
    GENERATED_BODY()

public:
    ABXNavigationLink();

    virtual void BeginPlay() override;

    // リンクの種類 (AI がどのアニメーションを使うかに影響)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|NavLink")
    EBXNavLinkType LinkType = EBXNavLinkType::Jump;

    // このリンクを使える AI クラスのタグ (未設定なら全 AI が使用可)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|NavLink")
    FName RequiredAITag;

    // リンク使用コスト (高いほど AI が回避する)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|NavLink",
        meta=(ClampMin="1.0"))
    float MovementCost = 1.0f;

    // 動的に有効/無効を切り替える (例: ドアが閉まっているとき無効)
    UFUNCTION(BlueprintCallable, Category="BX|NavLink")
    void SetLinkEnabled(bool bEnabled);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|NavLink")
    bool IsLinkEnabled() const { return bIsLinkEnabled; }

    // --- BlueprintImplementableEvent (アニメーション等) ---
    UFUNCTION(BlueprintImplementableEvent, Category="BX|NavLink")
    void BP_OnAgentUsedLink(AActor* Agent);

protected:
    UFUNCTION()
    void HandleSmartLinkReached(AActor* MovingActor, const FVector& Destination);

private:
    bool bIsLinkEnabled = true;
};
