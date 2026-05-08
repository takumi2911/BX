// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Location/FBXLocationRow.h"
#include "Data/Location/FBXSpawnPointRow.h"

#include "BXLocationManager.generated.h"

class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocationChanged, FName, NewLocationId);

// ロケーション / スポーン管理サブシステム (Sprint 25)
// GameInstance から自動生成される。
// レイド開始前にロケーションを SetCurrentLocation() でセットし、
// GetDefaultSpawnPoint() でスポーン座標を取得する。
UCLASS(BlueprintType)
class BX_API UBXLocationManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // --- DataTable 参照 (GameInstance Blueprint で設定) ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Location")
    TObjectPtr<UDataTable> LocationDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Location")
    TObjectPtr<UDataTable> SpawnPointDataTable;

    // --- ロケーションクエリ ---

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Location")
    TArray<FName> GetAllLocationIds() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Location")
    bool GetLocationRow(FName LocationId, FBXLocationRow& OutRow) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Location")
    TArray<FName> GetAvailableLocationIds() const;

    // --- 現在のロケーション ---

    UFUNCTION(BlueprintCallable, Category="BX|Location")
    bool SetCurrentLocation(FName LocationId);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Location")
    FName GetCurrentLocationId() const { return CurrentLocationId; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Location")
    bool HasCurrentLocation() const { return !CurrentLocationId.IsNone(); }

    // --- スポーンポイントクエリ ---

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Location")
    TArray<FName> GetSpawnPointIdsForLocation(FName LocationId) const;

    // デフォルトスポーンポイントを返す。なければ最初の有効なポイントを返す
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Location")
    bool GetDefaultSpawnPoint(FName LocationId, FBXSpawnPointRow& OutRow) const;

    // ランダムスポーンポイント (Available なもの限定)
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Location")
    bool GetRandomSpawnPoint(FName LocationId, FBXSpawnPointRow& OutRow) const;

    // --- Delegate ---

    UPROPERTY(BlueprintAssignable, Category="BX|Location")
    FOnLocationChanged OnLocationChanged;

protected:
    UPROPERTY(BlueprintReadOnly, Category="BX|Location")
    FName CurrentLocationId;
};
