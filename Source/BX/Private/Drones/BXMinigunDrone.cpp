// Copyright (c) takumi2911 - BlackoutExfil
#include "Drones/BXMinigunDrone.h"
#include "AI/BXDroneAIController.h"

ABXMinigunDrone::ABXMinigunDrone()
{
    PrimaryActorTick.bCanEverTick = true;
    // バースト制御を自前で持つため、基底の単発クールダウンを無効化
    AttackCooldownSec = 9999.0f;
}

void ABXMinigunDrone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsDead())
    {
        TickBurst(DeltaTime);
    }
}

void ABXMinigunDrone::StartBurst(AActor* Target)
{
    if (bInBurstCooldown || BurstRemaining > 0) { return; }
    BurstTarget = Target;
    BurstRemaining = BurstCount;
    BurstTimer = 0.0f;
}

void ABXMinigunDrone::TickBurst(float DeltaTime)
{
    if (bInBurstCooldown)
    {
        BurstCooldownTimer -= DeltaTime;
        if (BurstCooldownTimer <= 0.0f)
        {
            bInBurstCooldown = false;

            // クールダウン終了時に再びターゲットチェック
            ABXDroneAIController* AIC = Cast<ABXDroneAIController>(GetController());
            if (AIC && AIC->GetTargetActor() && DroneState == EBXDroneState::Attack)
            {
                StartBurst(AIC->GetTargetActor());
            }
        }
        return;
    }

    if (BurstRemaining <= 0) { return; }

    BurstTimer -= DeltaTime;
    if (BurstTimer > 0.0f) { return; }

    // 1 発撃つ
    AActor* Target = BurstTarget.Get();
    if (Target && !IsDead())
    {
        // LastFireTimeSec を直接リセットして基底の FireAtTarget クールダウンを回避
        LastFireTimeSec = -9999.0f;
        FireAtTarget(Target);
    }

    BurstRemaining--;
    BurstTimer = BurstIntervalSec;

    if (BurstRemaining <= 0)
    {
        bInBurstCooldown = true;
        BurstCooldownTimer = BurstCooldownSec;
    }
}
