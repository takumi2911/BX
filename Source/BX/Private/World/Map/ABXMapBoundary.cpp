// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.
#include "World/Map/ABXMapBoundary.h"
#include "Characters/APlayerCharacterBase.h"
#include "Characters/Components/AC_BX_HealthBodyParts.h"
#include "Data/BXEnums.h"
#include "Components/BoxComponent.h"

ABXMapBoundary::ABXMapBoundary()
{
    PrimaryActorTick.bCanEverTick = true;

    BoundaryVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundaryVolume"));
    BoundaryVolume->SetBoxExtent(FVector(5000.0f, 5000.0f, 2000.0f));
    BoundaryVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    RootComponent = BoundaryVolume;
}

void ABXMapBoundary::BeginPlay()
{
    Super::BeginPlay();
    BoundaryVolume->OnComponentBeginOverlap.AddDynamic(this, &ABXMapBoundary::OnVolumeBeginOverlap);
    BoundaryVolume->OnComponentEndOverlap.AddDynamic(this, &ABXMapBoundary::OnVolumeEndOverlap);
}

void ABXMapBoundary::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    for (auto& Pair : OutsideTimestamps)
    {
        if (!Pair.Key) { continue; }
        Pair.Value += DeltaTime;
        if (Pair.Value >= GracePeriodSec)
        {
            ApplyBoundaryAction(Pair.Key, DeltaTime);
        }
    }
}

void ABXMapBoundary::OnVolumeBeginOverlap(UPrimitiveComponent*, AActor* OtherActor,
    UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    if (APlayerCharacterBase* Player = Cast<APlayerCharacterBase>(OtherActor))
    {
        OutsideTimestamps.Remove(Player);
        BP_OnPlayerEnteredBoundary(Player);
    }
}

void ABXMapBoundary::OnVolumeEndOverlap(UPrimitiveComponent*, AActor* OtherActor,
    UPrimitiveComponent*, int32)
{
    if (APlayerCharacterBase* Player = Cast<APlayerCharacterBase>(OtherActor))
    {
        OutsideTimestamps.Add(Player, 0.0f);
        BP_OnPlayerExitedBoundary(Player);
    }
}

void ABXMapBoundary::ApplyBoundaryAction(APlayerCharacterBase* Player, float DeltaTime)
{
    switch (OutsideAction)
    {
    case EBXBoundaryAction::DamageOverTime:
        if (UAC_BX_HealthBodyParts* Health = Player->FindComponentByClass<UAC_BX_HealthBodyParts>())
        {
            Health->ApplyDamageToBodyPart(EBXBodyPart::Chest, DamagePerSecond * DeltaTime);
        }
        break;

    case EBXBoundaryAction::TeleportBack:
        {
            // BoundaryVolume の中心付近に戻す
            FVector Center = BoundaryVolume->GetComponentLocation();
            Player->TeleportTo(Center, Player->GetActorRotation());
            OutsideTimestamps.Remove(Player);
        }
        break;

    case EBXBoundaryAction::Kill:
        if (UAC_BX_HealthBodyParts* Health = Player->FindComponentByClass<UAC_BX_HealthBodyParts>())
        {
            Health->ApplyDamageToBodyPart(EBXBodyPart::Chest, 99999.0f);
        }
        OutsideTimestamps.Remove(Player);
        break;
    }
}
