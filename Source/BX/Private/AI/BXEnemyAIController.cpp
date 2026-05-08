// Copyright (c) takumi2911 - BlackoutExfil
#include "AI/BXEnemyAIController.h"
#include "Characters/BXEnemyCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "GameFramework/Character.h"

const FName ABXEnemyAIController::BB_TargetActor        = TEXT("TargetActor");
const FName ABXEnemyAIController::BB_LastKnownLocation  = TEXT("LastKnownLocation");
const FName ABXEnemyAIController::BB_PatrolTarget       = TEXT("PatrolTarget");
const FName ABXEnemyAIController::BB_AlertLevel         = TEXT("AlertLevel");
const FName ABXEnemyAIController::BB_bIsAlerted         = TEXT("bIsAlerted");
const FName ABXEnemyAIController::BB_bIsDead            = TEXT("bIsDead");

ABXEnemyAIController::ABXEnemyAIController()
{
    // Perception コンポーネント
    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius                    = 1500.0f;
    SightConfig->LoseSightRadius                = 1800.0f;
    SightConfig->PeripheralVisionAngleDegrees   = 60.0f;
    SightConfig->SetMaxAge(5.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies  = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 800.0f;
    HearingConfig->SetMaxAge(3.0f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies  = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->ConfigureSense(*HearingConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
    AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ABXEnemyAIController::OnPerceptionUpdated);
    // Signature: OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)

    // BTComponent は RunBehaviorTree() 呼び出し時に自動作成されるが、明示的に持っておく
    BTComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BTComponent"));
}

void ABXEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (Blackboard)
    {
        Blackboard->SetValueAsBool(BB_bIsDead, false);
        Blackboard->SetValueAsBool(BB_bIsAlerted, false);
        Blackboard->SetValueAsFloat(BB_AlertLevel, 0.0f);
    }
}

void ABXEnemyAIController::OnUnPossess()
{
    Super::OnUnPossess();
    BTComponent->StopTree();
}

void ABXEnemyAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor) { return; }

    // IsActive() = 感知中, !IsActive() = 感知ロスト
    const bool bCurrentlySensed = Stimulus.IsActive();
    UpdateEnemyStateFromPerception(Actor, bCurrentlySensed);
}

void ABXEnemyAIController::UpdateEnemyStateFromPerception(AActor* DetectedActor, bool bWasDetected)
{
    ABXEnemyCharacter* Enemy = GetEnemyCharacter();
    if (!Enemy || Enemy->IsDead()) { return; }

    if (bWasDetected)
    {
        SetTargetActor(DetectedActor);

        if (Blackboard)
        {
            Blackboard->SetValueAsVector(BB_LastKnownLocation, DetectedActor->GetActorLocation());
            Blackboard->SetValueAsBool(BB_bIsAlerted, true);
            Blackboard->SetValueAsFloat(BB_AlertLevel, 2.0f);
        }

        Enemy->SetEnemyState(EBXEnemyState::Combat);
    }
    else
    {
        // 視野を外れた: LastKnownLocation を保持したまま Alert に下げる
        if (GetTargetActor() == DetectedActor)
        {
            if (Blackboard)
            {
                Blackboard->SetValueAsFloat(BB_AlertLevel, 1.0f);
            }
            Enemy->SetEnemyState(EBXEnemyState::Alert);
        }
    }
}

ABXEnemyCharacter* ABXEnemyAIController::GetEnemyCharacter() const
{
    return Cast<ABXEnemyCharacter>(GetPawn());
}

void ABXEnemyAIController::SetTargetActor(AActor* Target)
{
    if (Blackboard)
    {
        Blackboard->SetValueAsObject(BB_TargetActor, Target);
    }
}

AActor* ABXEnemyAIController::GetTargetActor() const
{
    if (Blackboard)
    {
        return Cast<AActor>(Blackboard->GetValueAsObject(BB_TargetActor));
    }
    return nullptr;
}

void ABXEnemyAIController::ClearTarget()
{
    if (Blackboard)
    {
        Blackboard->ClearValue(BB_TargetActor);
        Blackboard->SetValueAsFloat(BB_AlertLevel, 0.0f);
        Blackboard->SetValueAsBool(BB_bIsAlerted, false);
    }

    ABXEnemyCharacter* Enemy = GetEnemyCharacter();
    if (Enemy && !Enemy->IsDead())
    {
        Enemy->SetEnemyState(EBXEnemyState::Patrol);
    }
}
