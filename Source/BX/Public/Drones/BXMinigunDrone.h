// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Drones/BXAttackDrone.h"

#include "BXMinigunDrone.generated.h"

// 機関銃ドローン (Sprint 27)
// 通常攻撃ドローンより高い連射速度でバースト射撃する。
// BurstCount 発撃ったら BurstCooldownSec 休止。
// FireAtTarget を override して連射ロジックを追加。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXMinigunDrone : public ABXAttackDrone
{
    GENERATED_BODY()

public:
    ABXMinigunDrone();

    virtual void Tick(float DeltaTime) override;

    // バースト 1 回あたりの発射数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone|Minigun")
    int32 BurstCount = 6;

    // バースト内の 1 発あたりのインターバル (秒)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone|Minigun")
    float BurstIntervalSec = 0.12f;

    // バースト終了後のクールダウン (秒)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone|Minigun")
    float BurstCooldownSec = 1.5f;

protected:
    int32 BurstRemaining = 0;
    float BurstTimer = 0.0f;
    float BurstCooldownTimer = 0.0f;
    bool bInBurstCooldown = false;
    TWeakObjectPtr<AActor> BurstTarget;

    void StartBurst(AActor* Target);
    void TickBurst(float DeltaTime);
};
