// Copyright (c) takumi2911 - BlackoutExfil
#include "AI/BXBTTask_AttackTarget.h"
#include "AI/BXEnemyAIController.h"
#include "Characters/BXEnemyCharacter.h"

UBXBTTask_AttackTarget::UBXBTTask_AttackTarget()
{
    NodeName = TEXT("BX Attack Target");
}

EBTNodeResult::Type UBXBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABXEnemyAIController* AIC = Cast<ABXEnemyAIController>(OwnerComp.GetAIOwner());
    if (!AIC) { return EBTNodeResult::Failed; }

    ABXEnemyCharacter* Enemy = AIC->GetEnemyCharacter();
    if (!Enemy || Enemy->IsDead()) { return EBTNodeResult::Failed; }

    AActor* Target = AIC->GetTargetActor();
    if (!Target) { return EBTNodeResult::Failed; }

    // FireAtTarget 内でクールダウン・精度チェックを行う
    Enemy->FireAtTarget(Target);

    return EBTNodeResult::Succeeded;
}
