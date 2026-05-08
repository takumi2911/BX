// Copyright (c) takumi2911 - BlackoutExfil
#include "Systems/Location/BXSpawnPointActor.h"
#include "Kismet/GameplayStatics.h"

ABXSpawnPointActor::ABXSpawnPointActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

ABXSpawnPointActor* ABXSpawnPointActor::FindBySpawnPointId(UObject* WorldContextObject, FName InSpawnPointId)
{
    if (!WorldContextObject || InSpawnPointId.IsNone()) { return nullptr; }

    TArray<AActor*> Actors;
    UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ABXSpawnPointActor::StaticClass(), Actors);

    for (AActor* Actor : Actors)
    {
        ABXSpawnPointActor* SpawnActor = Cast<ABXSpawnPointActor>(Actor);
        if (SpawnActor && SpawnActor->SpawnPointId == InSpawnPointId)
        {
            return SpawnActor;
        }
    }
    return nullptr;
}
