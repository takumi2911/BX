// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.
#include "World/Map/ABXOcclusionVolume.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

TArray<TWeakObjectPtr<ABXOcclusionVolume>> ABXOcclusionVolume::RegisteredVolumes;

ABXOcclusionVolume::ABXOcclusionVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    VolumeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("VolumeBox"));
    VolumeBox->SetBoxExtent(FVector(200.0f, 200.0f, 200.0f));
    VolumeBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RootComponent = VolumeBox;
}

void ABXOcclusionVolume::BeginPlay()
{
    Super::BeginPlay();
    RegisteredVolumes.Add(this);
}

void ABXOcclusionVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    RegisteredVolumes.Remove(this);
    Super::EndPlay(EndPlayReason);
}

bool ABXOcclusionVolume::ContainsLocation(FVector Location) const
{
    return VolumeBox->IsOverlappingActor(nullptr)
        ? false
        : VolumeBox->Bounds.GetBox().IsInsideOrOn(Location);
}

TArray<TWeakObjectPtr<ABXOcclusionVolume>>& ABXOcclusionVolume::GetRegisteredVolumes()
{
    return RegisteredVolumes;
}

void ABXOcclusionVolume::DrawDebug(float DeltaTime)
{
    if (!bShowDebugBox || !VolumeBox) { return; }
    DrawDebugBox(GetWorld(), VolumeBox->GetComponentLocation(),
        VolumeBox->GetScaledBoxExtent(), FColor::Red, false, DeltaTime + 0.01f);
}
