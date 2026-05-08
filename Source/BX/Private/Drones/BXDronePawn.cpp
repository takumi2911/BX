// Copyright (c) takumi2911 - BlackoutExfil
#include "Drones/BXDronePawn.h"
#include "Characters/Components/AC_BX_HealthBodyParts.h"
#include "Characters/Components/AC_BX_StatusEffects.h"
#include "Characters/Components/AC_BX_WeaponHandler.h"
#include "Data/Drone/FBXDroneArchetypeRow.h"
#include "Engine/DataTable.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

ABXDronePawn::ABXDronePawn()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetSphereRadius(40.0f);
    CollisionSphere->SetCollisionProfileName(TEXT("Pawn"));
    RootComponent = CollisionSphere;

    DroneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DroneMesh"));
    DroneMesh->SetupAttachment(RootComponent);
    DroneMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
    MovementComponent->MaxSpeed = 600.0f;
    MovementComponent->Acceleration = 1200.0f;

    HealthBodyParts = CreateDefaultSubobject<UAC_BX_HealthBodyParts>(TEXT("HealthBodyParts"));
    StatusEffects   = CreateDefaultSubobject<UAC_BX_StatusEffects>(TEXT("StatusEffects"));
    WeaponHandler   = CreateDefaultSubobject<UAC_BX_WeaponHandler>(TEXT("WeaponHandler"));
}

void ABXDronePawn::BeginPlay()
{
    Super::BeginPlay();
    InitFromArchetype();
    SetDroneState(EBXDroneState::Patrol);
}

void ABXDronePawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABXDronePawn::InitFromArchetype()
{
    if (!DroneArchetypeDataTable || ArchetypeId.IsNone()) { return; }

    const FBXDroneArchetypeRow* Row = DroneArchetypeDataTable->FindRow<FBXDroneArchetypeRow>(
        ArchetypeId, TEXT("ABXDronePawn::InitFromArchetype"));
    if (!Row) { return; }

    AttackCooldownSec = Row->AttackCooldownSec;
    BaseAccuracy      = Row->BaseAccuracy;
    AttackDamage      = Row->AttackDamage;

    if (MovementComponent)
    {
        MovementComponent->MaxSpeed = Row->PatrolSpeed;
    }
}

void ABXDronePawn::SetDroneState(EBXDroneState NewState)
{
    if (DroneState == NewState) { return; }
    DroneState = NewState;
    OnDroneStateChanged.Broadcast(this, NewState);

    if (!DroneArchetypeDataTable || ArchetypeId.IsNone()) { return; }
    const FBXDroneArchetypeRow* Row = DroneArchetypeDataTable->FindRow<FBXDroneArchetypeRow>(
        ArchetypeId, TEXT("SetDroneState"));
    if (!Row || !MovementComponent) { return; }

    switch (NewState)
    {
    case EBXDroneState::Patrol: MovementComponent->MaxSpeed = Row->PatrolSpeed; break;
    case EBXDroneState::Track:  MovementComponent->MaxSpeed = Row->TrackSpeed;  break;
    case EBXDroneState::Attack: MovementComponent->MaxSpeed = Row->AttackSpeed; break;
    default: break;
    }
}

float ABXDronePawn::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (IsDead()) { return 0.0f; }

    if (HealthBodyParts)
    {
        HealthBodyParts->ApplyDamageToBodyPart(EBXBodyPart::Chest, DamageAmount);
        if (HealthBodyParts->IsDead())
        {
            Die();
        }
    }
    return DamageAmount;
}

void ABXDronePawn::FireAtTarget(AActor* Target)
{
    if (!Target || IsDead() || !WeaponHandler) { return; }

    const float Now = GetWorld()->GetTimeSeconds();
    if (Now - LastFireTimeSec < AttackCooldownSec) { return; }
    LastFireTimeSec = Now;

    if (FMath::FRand() > BaseAccuracy) { return; }

    const FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    SetActorRotation(ToTarget.Rotation());
}

AActor* ABXDronePawn::GetNextPatrolPoint()
{
    if (PatrolPoints.IsEmpty()) { return nullptr; }
    PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
    return PatrolPoints[PatrolIndex];
}

void ABXDronePawn::Die()
{
    if (DroneState == EBXDroneState::Dead) { return; }
    DroneState = EBXDroneState::Dead;
    ApplyDestroyedState();
    OnDroneDied.Broadcast(this);
}

void ABXDronePawn::ApplyDestroyedState()
{
    if (MovementComponent)
    {
        MovementComponent->StopMovementImmediately();
        MovementComponent->SetActive(false);
    }
    if (CollisionSphere)
    {
        CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
