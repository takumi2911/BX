// Copyright (c) takumi2911 - BlackoutExfil
#include "Drones/BXAttackDrone.h"

ABXAttackDrone::ABXAttackDrone()
{
    // 攻撃ドローンはデフォルトで速め
}

void ABXAttackDrone::EngageTarget(AActor* Target)
{
    if (!Target || IsDead()) { return; }

    SetDroneState(EBXDroneState::Attack);

    const float Dist = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Dist <= EngageRange)
    {
        FireAtTarget(Target);
    }
    // 距離が遠い場合は BT の MoveToTarget タスクが先に接近させる
}
