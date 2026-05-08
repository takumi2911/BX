// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"

#include "BXEnemyAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class ABXEnemyCharacter;

// Sprint 24 敵 AI コントローラ
// BehaviorTree + AIPerception (視覚/聴覚) で制御。
// BT アセットは Blueprint で BXEnemyCharacter の AIControllerClass に設定する。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    ABXEnemyAIController();

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    // --- Perception ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|AI")
    TObjectPtr<UAIPerceptionComponent> AIPerception;

    UFUNCTION()
    void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    // --- Blackboard キー名 (Blueprint / BT Task と一致させること) ---

    static const FName BB_TargetActor;          // Object
    static const FName BB_LastKnownLocation;    // Vector
    static const FName BB_PatrolTarget;         // Object (AActor patrol point)
    static const FName BB_AlertLevel;           // Float  (0=Idle, 1=Alert, 2=Combat)
    static const FName BB_bIsAlerted;           // Bool
    static const FName BB_bIsDead;              // Bool

    // --- ユーティリティ ---

    UFUNCTION(BlueprintCallable, Category="BX|AI")
    ABXEnemyCharacter* GetEnemyCharacter() const;

    UFUNCTION(BlueprintCallable, Category="BX|AI")
    void SetTargetActor(AActor* Target);

    UFUNCTION(BlueprintCallable, Category="BX|AI")
    AActor* GetTargetActor() const;

    UFUNCTION(BlueprintCallable, Category="BX|AI")
    void ClearTarget();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|AI")
    TObjectPtr<UBehaviorTreeComponent> BTComponent;

    // 知覚更新時にステート遷移
    void UpdateEnemyStateFromPerception(AActor* DetectedActor, bool bWasDetected);
};
