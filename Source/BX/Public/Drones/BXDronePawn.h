// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Data/BXEnums.h"

#include "BXDronePawn.generated.h"

class UAC_BX_HealthBodyParts;
class UAC_BX_StatusEffects;
class UAC_BX_WeaponHandler;
class UFloatingPawnMovement;
class UStaticMeshComponent;
class USphereComponent;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDroneDied, ABXDronePawn*, Drone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDroneStateChanged, ABXDronePawn*, Drone, EBXDroneState, NewState);

// ドローン基底クラス (Sprint 26)
// ACharacter ではなく APawn ベース (飛行 + 物理なし移動)
// 偵察ドローン / 攻撃ドローンはこれを継承する
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXDronePawn : public APawn
{
    GENERATED_BODY()

public:
    ABXDronePawn();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    // --- コンポーネント ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Drone|Components")
    TObjectPtr<USphereComponent> CollisionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Drone|Components")
    TObjectPtr<UStaticMeshComponent> DroneMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Drone|Components")
    TObjectPtr<UFloatingPawnMovement> MovementComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Drone|Components")
    TObjectPtr<UAC_BX_HealthBodyParts> HealthBodyParts;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Drone|Components")
    TObjectPtr<UAC_BX_StatusEffects> StatusEffects;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Drone|Components")
    TObjectPtr<UAC_BX_WeaponHandler> WeaponHandler;

    // --- アーキタイプ ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone")
    FName ArchetypeId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone")
    TObjectPtr<UDataTable> DroneArchetypeDataTable;

    // --- 状態 ---

    UPROPERTY(BlueprintReadOnly, Category="BX|Drone")
    EBXDroneState DroneState = EBXDroneState::Idle;

    UFUNCTION(BlueprintCallable, Category="BX|Drone")
    void SetDroneState(EBXDroneState NewState);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Drone")
    bool IsDead() const { return DroneState == EBXDroneState::Dead; }

    // --- 攻撃 ---

    UFUNCTION(BlueprintCallable, Category="BX|Drone")
    void FireAtTarget(AActor* Target);

    // --- パトロール ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone|Patrol")
    TArray<TObjectPtr<AActor>> PatrolPoints;

    UFUNCTION(BlueprintCallable, Category="BX|Drone|Patrol")
    AActor* GetNextPatrolPoint();

    // --- Delegates ---

    UPROPERTY(BlueprintAssignable, Category="BX|Drone")
    FOnDroneDied OnDroneDied;

    UPROPERTY(BlueprintAssignable, Category="BX|Drone")
    FOnDroneStateChanged OnDroneStateChanged;

protected:
    int32 PatrolIndex = 0;
    float LastFireTimeSec = 0.0f;
    float AttackCooldownSec = 1.2f;
    float BaseAccuracy = 0.6f;
    float AttackDamage = 15.0f;

    void InitFromArchetype();
    void Die();
    void ApplyDestroyedState();
};
