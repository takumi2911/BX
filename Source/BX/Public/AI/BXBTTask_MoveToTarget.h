// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "BXBTTask_MoveToTarget.generated.h"

// BT タスク: BB_TargetActor の位置に向かって移動
// 攻撃射程内に入ったら Succeeded を返す
UCLASS()
class BX_API UBXBTTask_MoveToTarget : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBXBTTask_MoveToTarget();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    // 目標との距離がこれ以下になったら成功
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|AI")
    float AcceptanceRadius = 700.0f;

    // 経路再計算間隔
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|AI")
    float PathUpdateInterval = 0.5f;

private:
    float TimeSinceLastPath = 0.0f;
};
