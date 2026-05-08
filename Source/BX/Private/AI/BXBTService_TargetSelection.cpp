// Copyright (c) takumi2911 - BlackoutExfil
#include "AI/BXBTService_TargetSelection.h"
#include "AI/BXEnemyAIController.h"
#include "Characters/BXEnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UBXBTService_TargetSelection::UBXBTService_TargetSelection()
{
    NodeName = TEXT("BX Target Selection");
    Interval = 0.25f; // 250ms ごとに更新
}

void UBXBTService_TargetSelection::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    ABXEnemyAIController* AIC = Cast<ABXEnemyAIController>(OwnerComp.GetAIOwner());
    if (!AIC) { return; }

    ABXEnemyCharacter* Enemy = AIC->GetEnemyCharacter();
    if (!Enemy || Enemy->IsDead()) { return; }

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) { return; }

    // Perception がすでにターゲットを持っていればそちらを優先
    if (AIC->GetTargetActor())
    {
        AActor* Current = AIC->GetTargetActor();
        BB->SetValueAsVector(ABXEnemyAIController::BB_LastKnownLocation,
            Current->GetActorLocation());
        return;
    }

    // フォールバック: プレイヤーキャラクタを距離で探す
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Enemy->GetWorld(), 0);
    if (!Player) { return; }

    const float Dist = FVector::Dist(Enemy->GetActorLocation(), Player->GetActorLocation());
    if (Dist <= FallbackSearchRadius)
    {
        AIC->SetTargetActor(Player);
        BB->SetValueAsBool(ABXEnemyAIController::BB_bIsAlerted, true);
        BB->SetValueAsFloat(ABXEnemyAIController::BB_AlertLevel, 2.0f);
        Enemy->SetEnemyState(EBXEnemyState::Combat);
    }
}
