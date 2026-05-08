// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "BXBTTask_AttackTarget.generated.h"

// BT タスク: BB_TargetActor に向けて FireAtTarget を呼ぶ
// クールダウンは ABXEnemyCharacter 側で管理するため、タスク自体は即 Succeeded を返す
UCLASS()
class BX_API UBXBTTask_AttackTarget : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBXBTTask_AttackTarget();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
