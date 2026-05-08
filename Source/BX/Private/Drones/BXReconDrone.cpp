// Copyright (c) takumi2911 - BlackoutExfil
#include "Drones/BXReconDrone.h"
#include "Characters/BXEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"

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
    TArray<AActor*> NearbyActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABXEnemyCharacter::StaticClass(), NearbyActors);

    for (AActor* Actor : NearbyActors)
    {
        ABXEnemyCharacter* Enemy = Cast<ABXEnemyCharacter>(Actor);
        if (!Enemy || Enemy->IsDead()) { continue; }

        const float Dist = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
        if (Dist <= AlertPropagationRadius)
        {
            Enemy->SetEnemyState(EBXEnemyState::Alert);
        }
    }
}
