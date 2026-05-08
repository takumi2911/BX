// Copyright (c) takumi2911 - BlackoutExfil
#include "Systems/BXAlertSubsystem.h"
#include "Characters/BXEnemyCharacter.h"
#include "Drones/BXDronePawn.h"

void UBXAlertSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UBXAlertSubsystem::RegisterEnemy(ABXEnemyCharacter* Enemy)
{
    if (Enemy && !RegisteredEnemies.Contains(Enemy))
    {
        RegisteredEnemies.Add(Enemy);
    }
}

void UBXAlertSubsystem::UnregisterEnemy(ABXEnemyCharacter* Enemy)
{
    RegisteredEnemies.RemoveSwap(Enemy);
}

void UBXAlertSubsystem::RegisterDrone(ABXDronePawn* Drone)
{
    if (Drone && !RegisteredDrones.Contains(Drone))
    {
        RegisteredDrones.Add(Drone);
    }
}

void UBXAlertSubsystem::UnregisterDrone(ABXDronePawn* Drone)
{
    RegisteredDrones.RemoveSwap(Drone);
}

void UBXAlertSubsystem::BroadcastAlertToEnemies(AActor* Source, FVector Location, float Radius)
{
    CleanupStaleEntries();

    for (const TWeakObjectPtr<ABXEnemyCharacter>& WeakEnemy : RegisteredEnemies)
    {
        ABXEnemyCharacter* Enemy = WeakEnemy.Get();
        if (!Enemy || Enemy->IsDead() || Enemy == Source) { continue; }

        const float Dist = FVector::Dist(Location, Enemy->GetActorLocation());
        if (Dist <= Radius)
        {
            Enemy->SetEnemyState(EBXEnemyState::Alert);
        }
    }
}

void UBXAlertSubsystem::BroadcastAlertToDrones(AActor* Source, FVector Location, float Radius)
{
    CleanupStaleEntries();

    for (const TWeakObjectPtr<ABXDronePawn>& WeakDrone : RegisteredDrones)
    {
        ABXDronePawn* Drone = WeakDrone.Get();
        if (!Drone || Drone->IsDead() || Drone == Source) { continue; }

        const float Dist = FVector::Dist(Location, Drone->GetActorLocation());
        if (Dist <= Radius)
        {
            Drone->SetDroneState(EBXDroneState::Track);
        }
    }
}

void UBXAlertSubsystem::BroadcastAlertAll(AActor* Source, FVector Location, float Radius)
{
    BroadcastAlertToEnemies(Source, Location, Radius);
    BroadcastAlertToDrones(Source, Location, Radius);
}

TArray<ABXEnemyCharacter*> UBXAlertSubsystem::GetEnemiesInRadius(FVector Location, float Radius) const
{
    TArray<ABXEnemyCharacter*> Result;
    for (const TWeakObjectPtr<ABXEnemyCharacter>& WeakEnemy : RegisteredEnemies)
    {
        ABXEnemyCharacter* Enemy = WeakEnemy.Get();
        if (!Enemy || Enemy->IsDead()) { continue; }
        if (FVector::Dist(Location, Enemy->GetActorLocation()) <= Radius)
        {
            Result.Add(Enemy);
        }
    }
    return Result;
}

void UBXAlertSubsystem::CleanupStaleEntries()
{
    RegisteredEnemies.RemoveAll([](const TWeakObjectPtr<ABXEnemyCharacter>& W) { return !W.IsValid(); });
    RegisteredDrones.RemoveAll([](const TWeakObjectPtr<ABXDronePawn>& W) { return !W.IsValid(); });
}
