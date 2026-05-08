// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Drones/BXDronePawn.h"

#include "BXAttackDrone.generated.h"

// 攻撃ドローン (Sprint 26)
// プレイヤーを発見したら自律的に追跡 + 射撃する。
// FireAtTarget は基底 BXDronePawn の実装を使用。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXAttackDrone : public ABXDronePawn
{
    GENERATED_BODY()

public:
    ABXAttackDrone();

    // BT Task から呼ぶ: ターゲットに近づきながら発砲
    UFUNCTION(BlueprintCallable, Category="BX|Drone|Attack")
    void EngageTarget(AActor* Target);

    // 攻撃開始距離 (これ以上離れているときは先に接近)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Drone|Attack")
    float EngageRange = 1000.0f;
};
