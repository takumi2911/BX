// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "BXAlertSubsystem.generated.h"

class ABXEnemyCharacter;
class ABXDronePawn;

// 警報伝播サブシステム (Sprint 27)
// GetAllActorsOfClass の代替。
// 敵 / ドローンが BeginPlay で RegisterEnemy/RegisterDrone を呼び、
// BroadcastAlert() で近傍 AI に一括通知する。
// WorldSubsystem なのでレベル切り替え時に自動クリーンアップされる。
UCLASS(BlueprintType)
class BX_API UBXAlertSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // --- 登録 ---

    UFUNCTION(BlueprintCallable, Category="BX|Alert")
    void RegisterEnemy(ABXEnemyCharacter* Enemy);

    UFUNCTION(BlueprintCallable, Category="BX|Alert")
    void UnregisterEnemy(ABXEnemyCharacter* Enemy);

    UFUNCTION(BlueprintCallable, Category="BX|Alert")
    void RegisterDrone(ABXDronePawn* Drone);

    UFUNCTION(BlueprintCallable, Category="BX|Alert")
    void UnregisterDrone(ABXDronePawn* Drone);

    // --- アラート伝播 ---

    // Source から Radius 内の全 ABXEnemyCharacter に Alert ステート遷移を伝える
    UFUNCTION(BlueprintCallable, Category="BX|Alert")
    void BroadcastAlertToEnemies(AActor* Source, FVector Location, float Radius);

    // Source から Radius 内の全 ABXDronePawn に Track ステート遷移を伝える
    UFUNCTION(BlueprintCallable, Category="BX|Alert")
    void BroadcastAlertToDrones(AActor* Source, FVector Location, float Radius);

    // 敵 + ドローン 両方に通知
    UFUNCTION(BlueprintCallable, Category="BX|Alert")
    void BroadcastAlertAll(AActor* Source, FVector Location, float Radius);

    // --- クエリ ---

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Alert")
    TArray<ABXEnemyCharacter*> GetEnemiesInRadius(FVector Location, float Radius) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Alert")
    int32 GetRegisteredEnemyCount() const { return RegisteredEnemies.Num(); }

protected:
    TArray<TWeakObjectPtr<ABXEnemyCharacter>> RegisteredEnemies;
    TArray<TWeakObjectPtr<ABXDronePawn>>      RegisteredDrones;

    void CleanupStaleEntries();
};
