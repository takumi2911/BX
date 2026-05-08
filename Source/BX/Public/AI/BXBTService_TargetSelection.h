// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"

#include "BXBTService_TargetSelection.generated.h"

// BT サービス: 毎フレーム近傍プレイヤーを BB_TargetActor に書き込む
// BT の Selector ルートにアタッチして常時実行させる
UCLASS()
class BX_API UBXBTService_TargetSelection : public UBTService
{
    GENERATED_BODY()

public:
    UBXBTService_TargetSelection();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    // 感知範囲外でも強制的にターゲットを探す最大距離
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|AI")
    float FallbackSearchRadius = 2000.0f;
};
