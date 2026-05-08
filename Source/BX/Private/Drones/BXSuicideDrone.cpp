// Copyright (c) takumi2911 - BlackoutExfil
#include "Drones/BXSuicideDrone.h"
#include "AI/BXDroneAIController.h"
#include "Kismet/GameplayStatics.h"

ABXSuicideDrone::ABXSuicideDrone()
{
    PrimaryActorTick.bCanEverTick = true;
    // 自爆ドローンは攻撃クールダウン不要 (接近して自爆するだけ)
    AttackCooldownSec = 9999.0f;
    EngageRange = ExplodeRadius * 2.0f;
}

void ABXSuicideDrone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsDead() && !bHasExploded && DroneState == EBXDroneState::Attack)
    {
        CheckProximityToTarget();
    }
}

void ABXSuicideDrone::CheckProximityToTarget()
{
    ABXDroneAIController* AIC = Cast<ABXDroneAIController>(GetController());
    if (!AIC) { return; }

    AActor* Target = AIC->GetTargetActor();
    if (!Target) { return; }

    const float Dist = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Dist <= ExplodeRadius)
    {
        Explode();
    }
}

void ABXSuicideDrone::Explode()
{
    if (bHasExploded) { return; }
    bHasExploded = true;

    // 爆発ダメージを周囲に適用
    UGameplayStatics::ApplyRadialDamage(
        GetWorld(),
        ExplosionDamage,
        GetActorLocation(),
        ExplosionRadius,
        UDamageType::StaticClass(),
        TArray<AActor*>{ this },  // 自身をダメージ除外リストに入れる
        this,
        GetController(),
        true  // bDoFullDamage = 中心はフル、減衰あり
    );

    // ドローン自身を破壊
    Die();
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    // 一定時間後に Actor を削除 (爆発エフェクト終了後)
    SetLifeSpan(2.0f);
}
