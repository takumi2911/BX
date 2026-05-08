// Copyright (c) takumi2911 - BlackoutExfil
#include "Drones/BXReconDrone.h"
#include "Characters/BXEnemyCharacter.h"
#include "Systems/BXAlertSubsystem.h"

ABXReconDrone::ABXReconDrone()
{
    // 偵察ドローンは攻撃しない
}

void ABXReconDrone::NotifyPlayerSpotted(AActor* SpottedActor)
{
    if (!SpottedActor || IsDead()) { return; }

    SetDroneState(EBXDroneState::Track);
    OnPlayerSpotted.Broadcast(this, SpottedActor);
    PropagateAlertToNearbyEnemies(SpottedActor);
}

void ABXReconDrone::PropagateAlertToNearbyEnemies(AActor* SpottedActor)
{
    if (UBXAlertSubsystem* AlertSys = GetWorld()->GetSubsystem<UBXAlertSubsystem>())
    {
        AlertSys->BroadcastAlertToEnemies(this, GetActorLocation(), AlertPropagationRadius);
    }
}
