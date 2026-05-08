// Copyright (c) takumi2911 - BlackoutExfil
#include "AI/BXDroneAIController.h"
#include "Drones/BXDronePawn.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

const FName ABXDroneAIController::BB_TargetActor       = TEXT("TargetActor");
const FName ABXDroneAIController::BB_LastKnownLocation = TEXT("LastKnownLocation");
const FName ABXDroneAIController::BB_PatrolTarget      = TEXT("PatrolTarget");
const FName ABXDroneAIController::BB_DroneState        = TEXT("DroneState");
const FName ABXDroneAIController::BB_bIsTracking       = TEXT("bIsTracking");

ABXDroneAIController::ABXDroneAIController()
{
    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius                    = 2500.0f;
    SightConfig->LoseSightRadius                = 3000.0f;
    SightConfig->PeripheralVisionAngleDegrees   = 90.0f;
    SightConfig->SetMaxAge(5.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies  = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
    AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ABXDroneAIController::OnPerceptionUpdated);

    BTComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BTComponent"));
}

void ABXDroneAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (Blackboard)
    {
        Blackboard->SetValueAsBool(BB_bIsTracking, false);
    }
}

void ABXDroneAIController::OnUnPossess()
{
    Super::OnUnPossess();
    BTComponent->StopTree();
}

void ABXDroneAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor) { return; }

    ABXDronePawn* Drone = GetDronePawn();
    if (!Drone || Drone->IsDead()) { return; }

    if (Stimulus.IsActive())
    {
        SetTargetActor(Actor);
        if (Blackboard)
        {
            Blackboard->SetValueAsVector(BB_LastKnownLocation, Actor->GetActorLocation());
            Blackboard->SetValueAsBool(BB_bIsTracking, true);
        }
        Drone->SetDroneState(EBXDroneState::Track);
    }
    else
    {
        if (GetTargetActor() == Actor)
        {
            ClearTarget();
        }
    }
}

ABXDronePawn* ABXDroneAIController::GetDronePawn() const
{
    return Cast<ABXDronePawn>(GetPawn());
}

void ABXDroneAIController::SetTargetActor(AActor* Target)
{
    if (Blackboard)
    {
        Blackboard->SetValueAsObject(BB_TargetActor, Target);
    }
}

AActor* ABXDroneAIController::GetTargetActor() const
{
    if (Blackboard)
    {
        return Cast<AActor>(Blackboard->GetValueAsObject(BB_TargetActor));
    }
    return nullptr;
}

void ABXDroneAIController::ClearTarget()
{
    if (Blackboard)
    {
        Blackboard->ClearValue(BB_TargetActor);
        Blackboard->SetValueAsBool(BB_bIsTracking, false);
    }

    ABXDronePawn* Drone = GetDronePawn();
    if (Drone && !Drone->IsDead())
    {
        Drone->SetDroneState(EBXDroneState::Patrol);
    }
}
