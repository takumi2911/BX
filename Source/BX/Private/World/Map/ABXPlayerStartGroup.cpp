// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.
#include "World/Map/ABXPlayerStartGroup.h"
#include "GameFramework/PlayerStart.h"

ABXPlayerStartGroup::ABXPlayerStartGroup()
{
    PrimaryActorTick.bCanEverTick = false;
}

APlayerStart* ABXPlayerStartGroup::PickNextSpawnPoint()
{
    TArray<APlayerStart*> Valid;
    for (auto& Ptr : SpawnPoints)
    {
        if (Ptr.Get()) { Valid.Add(Ptr.Get()); }
    }
    if (Valid.IsEmpty()) { return nullptr; }

    switch (Strategy)
    {
    case EBXSpawnStrategy::Random:
        return Valid[FMath::RandRange(0, Valid.Num() - 1)];

    case EBXSpawnStrategy::Sequential:
        {
            APlayerStart* Pt = Valid[SequentialIndex % Valid.Num()];
            SequentialIndex++;
            return Pt;
        }

    case EBXSpawnStrategy::LeastRecent:
        {
            APlayerStart* Best = nullptr;
            float OldestTime = TNumericLimits<float>::Max();
            for (APlayerStart* Pt : Valid)
            {
                const float LastTime = LastUsedTimestamps.FindRef(Pt);
                if (LastTime < OldestTime)
                {
                    OldestTime = LastTime;
                    Best = Pt;
                }
            }
            return Best;
        }
    }
    return nullptr;
}

void ABXPlayerStartGroup::RecordSpawnUsed(APlayerStart* UsedPoint)
{
    if (UsedPoint && GetWorld())
    {
        LastUsedTimestamps.Add(UsedPoint, GetWorld()->GetTimeSeconds());
    }
}

bool ABXPlayerStartGroup::HasValidSpawnPoints() const
{
    for (const auto& Ptr : SpawnPoints)
    {
        if (Ptr.Get()) { return true; }
    }
    return false;
}
