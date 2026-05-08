// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"

#include "BXDroneAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
class UAISenseConfig_Sight;
class ABXDronePawn;

// Sprint 26 ドローン AI コントローラ
// 敵 AIController と同構造だが、飛行体なので NavMesh を使わない移動が前提。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXDroneAIController : public AAIController
{
    GENERATED_BODY()

public:
    ABXDroneAIController();

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    // --- Perception ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|AI|Drone")
    TObjectPtr<UAIPerceptionComponent> AIPerception;

    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    // --- Blackboard キー名 ---

    static const FName BB_TargetActor;
    static const FName BB_LastKnownLocation;
    static const FName BB_PatrolTarget;
    static const FName BB_DroneState;   // FName (Idle/Patrol/Track/Attack/Dead)
    static const FName BB_bIsTracking;

    // --- ユーティリティ ---

    UFUNCTION(BlueprintCallable, Category="BX|AI|Drone")
    ABXDronePawn* GetDronePawn() const;

    UFUNCTION(BlueprintCallable, Category="BX|AI|Drone")
    void SetTargetActor(AActor* Target);

    UFUNCTION(BlueprintCallable, Category="BX|AI|Drone")
    AActor* GetTargetActor() const;

    UFUNCTION(BlueprintCallable, Category="BX|AI|Drone")
    void ClearTarget();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|AI|Drone")
    TObjectPtr<UBehaviorTreeComponent> BTComponent;
};
