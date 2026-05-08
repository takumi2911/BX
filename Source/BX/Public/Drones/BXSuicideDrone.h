// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Drones/BXAttackDrone.h"

#include "BXSuicideDrone.generated.h"

// 自爆ドローン (Sprint 27)
// ターゲットに一定距離まで接近すると自爆。周囲に爆発ダメージを与える。
// BT 側は通常の AttackDrone と同じフロー (MoveTo -> EngageTarget)。
// ExplodeRadius 内に入ったとき Explode() が自動で呼ばれる。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXSuicideDrone : public ABXAttackDrone
{
    GENERATED_BODY()

public:
    ABXSuicideDrone();

    virtual void Tick(float DeltaTime) override;

    // --- 自爆パラメータ ---

    // ターゲットがこの距離以内に入ったら自爆
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone|Suicide")
    float ExplodeRadius = 200.0f;

    // 爆発の影響範囲 (cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone|Suicide")
    float ExplosionRadius = 400.0f;

    // 爆発の最大ダメージ (中心付近)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone|Suicide")
    float ExplosionDamage = 80.0f;

    // 爆発時の自身へのダメージは無視 (常に true)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone|Suicide")
    bool bExplodeOnDeath = true;

    UFUNCTION(BlueprintCallable, Category="BX|Drone|Suicide")
    void Explode();

protected:
    bool bHasExploded = false;

    void CheckProximityToTarget();
};
