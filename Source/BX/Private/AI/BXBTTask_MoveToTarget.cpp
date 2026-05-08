// Copyright (c) takumi2911 - BlackoutExfil
#include "AI/BXBTTask_MoveToTarget.h"
#include "AI/BXEnemyAIController.h"
#include "Characters/BXEnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBXBTTask_MoveToTarget::UBXBTTask_MoveToTarget()
{
    NodeName = TEXT("BX Move To Target");
    bNotifyTick = true;
}

EBTNodeResult::Type UBXBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABXEnemyAIController* AIC = Cast<ABXEnemyAIController>(OwnerComp.GetAIOwner());
    if (!AIC) { return EBTNodeResult::Failed; }

    ABXEnemyCharacter* Enemy = AIC->GetEnemyCharacter();
    if (!Enemy || Enemy->IsDead()) { return EBTNodeResult::Failed; }

    AActor* Target = AIC->GetTargetActor();
    if (!Target) { return EBTNodeResult::Failed; }

    const float Dist = FVector::Dist(Enemy->GetActorLocation(), Target->GetActorLocation());
    if (Dist <= AcceptanceRadius) { return EBTNodeResult::Succeeded; }

    FAIMoveRequest MoveReq(Target);
    MoveReq.SetAcceptanceRadius(AcceptanceRadius);
    MoveReq.SetUsePathfinding(true);

    const FPathFollowingRequestResult Result = AIC->MoveTo(MoveReq);
    if (Result.Code == EPathFollowingRequestResult::Failed)
    {
        return EBTNodeResult::Failed;
    }

    TimeSinceLastPath = 0.0f;
    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBXBTTask_MoveToTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABXEnemyAIController* AIC = Cast<ABXEnemyAIController>(OwnerComp.GetAIOwner());
    if (AIC) { AIC->StopMovement(); }
    return EBTNodeResult::Aborted;
}

void UBXBTTask_MoveToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    ABXEnemyAIController* AIC = Cast<ABXEnemyAIController>(OwnerComp.GetAIOwner());
    if (!AIC) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    ABXEnemyCharacter* Enemy = AIC->GetEnemyCharacter();
    if (!Enemy || Enemy->IsDead()) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    AActor* Target = AIC->GetTargetActor();
    if (!Target) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    const float Dist = FVector::Dist(Enemy->GetActorLocation(), Target->GetActorLocation());
    if (Dist <= AcceptanceRadius)
    {
        AIC->StopMovement();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // 経路を定期的に再計算 (ターゲットが動く場合)
    TimeSinceLastPath += DeltaSeconds;
    if (TimeSinceLastPath >= PathUpdateInterval)
    {
        TimeSinceLastPath = 0.0f;
        FAIMoveRequest MoveReq(Target);
        MoveReq.SetAcceptanceRadius(AcceptanceRadius);
        AIC->MoveTo(MoveReq);
    }
}
