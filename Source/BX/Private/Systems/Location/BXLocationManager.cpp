// Copyright (c) takumi2911 - BlackoutExfil
#include "Systems/Location/BXLocationManager.h"
#include "Engine/DataTable.h"

void UBXLocationManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

TArray<FName> UBXLocationManager::GetAllLocationIds() const
{
    TArray<FName> Result;
    if (!LocationDataTable) { return Result; }
    return LocationDataTable->GetRowNames();
}

bool UBXLocationManager::GetLocationRow(FName LocationId, FBXLocationRow& OutRow) const
{
    if (!LocationDataTable || LocationId.IsNone()) { return false; }
    const FBXLocationRow* Row = LocationDataTable->FindRow<FBXLocationRow>(
        LocationId, TEXT("UBXLocationManager::GetLocationRow"));
    if (!Row) { return false; }
    OutRow = *Row;
    return true;
}

TArray<FName> UBXLocationManager::GetAvailableLocationIds() const
{
    TArray<FName> Result;
    if (!LocationDataTable) { return Result; }

    for (const FName& Id : LocationDataTable->GetRowNames())
    {
        const FBXLocationRow* Row = LocationDataTable->FindRow<FBXLocationRow>(Id, TEXT("GetAvailableLocationIds"));
        if (Row && Row->bIsAvailable)
        {
            Result.Add(Id);
        }
    }
    return Result;
}

bool UBXLocationManager::SetCurrentLocation(FName LocationId)
{
    if (!LocationDataTable || LocationId.IsNone()) { return false; }

    const FBXLocationRow* Row = LocationDataTable->FindRow<FBXLocationRow>(
        LocationId, TEXT("SetCurrentLocation"));
    if (!Row || !Row->bIsAvailable) { return false; }

    CurrentLocationId = LocationId;
    OnLocationChanged.Broadcast(LocationId);
    return true;
}

TArray<FName> UBXLocationManager::GetSpawnPointIdsForLocation(FName LocationId) const
{
    TArray<FName> Result;
    if (!SpawnPointDataTable || LocationId.IsNone()) { return Result; }

    for (const FName& Id : SpawnPointDataTable->GetRowNames())
    {
        const FBXSpawnPointRow* Row = SpawnPointDataTable->FindRow<FBXSpawnPointRow>(Id, TEXT("GetSpawnPointIds"));
        if (Row && Row->LocationId == LocationId && Row->bIsAvailable)
        {
            Result.Add(Id);
        }
    }
    return Result;
}

bool UBXLocationManager::GetDefaultSpawnPoint(FName LocationId, FBXSpawnPointRow& OutRow) const
{
    if (!SpawnPointDataTable || LocationId.IsNone()) { return false; }

    const FBXSpawnPointRow* FirstAvailable = nullptr;

    for (const FName& Id : SpawnPointDataTable->GetRowNames())
    {
        const FBXSpawnPointRow* Row = SpawnPointDataTable->FindRow<FBXSpawnPointRow>(Id, TEXT("GetDefaultSpawnPoint"));
        if (!Row || Row->LocationId != LocationId || !Row->bIsAvailable) { continue; }

        if (Row->bIsDefault)
        {
            OutRow = *Row;
            return true;
        }
        if (!FirstAvailable)
        {
            FirstAvailable = Row;
        }
    }

    if (FirstAvailable)
    {
        OutRow = *FirstAvailable;
        return true;
    }
    return false;
}

bool UBXLocationManager::GetRandomSpawnPoint(FName LocationId, FBXSpawnPointRow& OutRow) const
{
    TArray<FName> Available = GetSpawnPointIdsForLocation(LocationId);
    if (Available.IsEmpty()) { return false; }

    const int32 Idx = FMath::RandRange(0, Available.Num() - 1);
    const FBXSpawnPointRow* Row = SpawnPointDataTable->FindRow<FBXSpawnPointRow>(
        Available[Idx], TEXT("GetRandomSpawnPoint"));
    if (!Row) { return false; }

    OutRow = *Row;
    return true;
}
