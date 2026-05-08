// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.
#include "World/Map/ABXAmbientSoundEmitter.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"

ABXAmbientSoundEmitter::ABXAmbientSoundEmitter()
{
    PrimaryActorTick.bCanEverTick = false;

    DebugSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DebugSphere"));
    DebugSphere->SetSphereRadius(50.0f);
    DebugSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DebugSphere->bHiddenInGame = true;
    RootComponent = DebugSphere;

    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    AudioComponent->SetupAttachment(RootComponent);
    AudioComponent->bAutoActivate = false;
}

void ABXAmbientSoundEmitter::BeginPlay()
{
    Super::BeginPlay();

    if (!AmbientSound) { return; }
    AudioComponent->SetSound(AmbientSound);
    AudioComponent->SetVolumeMultiplier(VolumeMultiplier);

    switch (SoundType)
    {
    case EBXAmbientSoundType::Continuous:
        AudioComponent->bIsUISound = false;
        AudioComponent->Play();
        break;
    case EBXAmbientSoundType::OneShot:
        AudioComponent->Play();
        break;
    case EBXAmbientSoundType::RandomLoop:
        ScheduleNextPlay();
        break;
    }
}

void ABXAmbientSoundEmitter::PlaySound()
{
    if (AudioComponent && AmbientSound)
    {
        AudioComponent->Play();
    }
}

void ABXAmbientSoundEmitter::StopSound()
{
    if (AudioComponent)
    {
        AudioComponent->Stop();
    }
}

void ABXAmbientSoundEmitter::ScheduleNextPlay()
{
    const float Delay = FMath::FRandRange(MinInterval, MaxInterval);
    GetWorldTimerManager().SetTimer(RandomLoopTimer, [this]()
    {
        PlaySound();
        ScheduleNextPlay();
    }, Delay, false);
}
