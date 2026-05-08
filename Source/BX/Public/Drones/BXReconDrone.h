// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Drones/BXDronePawn.h"

#include "BXReconDrone.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerSpotted, ABXReconDrone*, Drone, AActor*, SpottedActor);

// 偵察ドローン (Sprint 26)
// プレイヤーを発見したら OnPlayerSpotted を broadcast して近傍の敵 AI に通知する。
// 自身は攻撃しない (WeaponHandler は使わない)。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXReconDrone : public ABXDronePawn
{
    GENERATED_BODY()

public:
    ABXReconDrone();

    // 感知範囲内のプレイヤーを発見したとき (BT Task or Perception から呼ぶ)
    UFUNCTION(BlueprintCallable, Category="BX|Drone|Recon")
    void NotifyPlayerSpotted(AActor* SpottedActor);

    // 近隣敵 AI にアラートを伝搬する範囲 (cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone|Recon")
    float AlertPropagationRadius = 3000.0f;

    UPROPERTY(BlueprintAssignable, Category="BX|Drone|Recon")
    FOnPlayerSpotted OnPlayerSpotted;

protected:
    // 近隣 ABXEnemyCharacter に Combat ステート遷移を伝える
    void PropagateAlertToNearbyEnemies(AActor* SpottedActor);
};
