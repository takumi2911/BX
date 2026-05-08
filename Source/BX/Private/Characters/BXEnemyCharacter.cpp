// Copyright (c) takumi2911 - BlackoutExfil
#include "Characters/BXEnemyCharacter.h"
#include "Characters/Components/AC_BX_HealthBodyParts.h"
#include "Characters/Components/AC_BX_ArmorEquipment.h"
#include "Characters/Components/AC_BX_StatusEffects.h"
#include "Characters/Components/AC_BX_WeaponHandler.h"
#include "Data/Enemy/FBXEnemyArchetypeRow.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Systems/BXAlertSubsystem.h"

ABXEnemyCharacter::ABXEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    HealthBodyParts = CreateDefaultSubobject<UAC_BX_HealthBodyParts>(TEXT("HealthBodyParts"));
    ArmorEquipment  = CreateDefaultSubobject<UAC_BX_ArmorEquipment>(TEXT("ArmorEquipment"));
    StatusEffects   = CreateDefaultSubobject<UAC_BX_StatusEffects>(TEXT("StatusEffects"));
    WeaponHandler   = CreateDefaultSubobject<UAC_BX_WeaponHandler>(TEXT("WeaponHandler"));
}

void ABXEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
    InitFromArchetype();
    SetEnemyState(EBXEnemyState::Patrol);

    if (UBXAlertSubsystem* AlertSys = GetWorld()->GetSubsystem<UBXAlertSubsystem>())
    {
        AlertSys->RegisterEnemy(this);
    }
}

void ABXEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABXEnemyCharacter::InitFromArchetype()
{
    if (!EnemyArchetypeDataTable || ArchetypeId.IsNone()) { return; }

    const FBXEnemyArchetypeRow* Row = EnemyArchetypeDataTable->FindRow<FBXEnemyArchetypeRow>(
        ArchetypeId, TEXT("ABXEnemyCharacter::InitFromArchetype"));
    if (!Row) { return; }

    AttackCooldownSec = Row->AttackCooldownSec;
    BaseAccuracy      = Row->BaseAccuracy;

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = Row->WalkSpeed;
    }

    if (ArmorEquipment && Row->InitialArmorItemId != NAME_None)
    {
        ArmorEquipment->EquipArmorItem(Row->InitialArmorItemId);
    }
}

void ABXEnemyCharacter::SetEnemyState(EBXEnemyState NewState)
{
    if (EnemyState == NewState) { return; }
    EnemyState = NewState;
    OnEnemyStateChanged.Broadcast(this, NewState);

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        if (!EnemyArchetypeDataTable || ArchetypeId.IsNone()) { return; }
        const FBXEnemyArchetypeRow* Row = EnemyArchetypeDataTable->FindRow<FBXEnemyArchetypeRow>(
            ArchetypeId, TEXT("SetEnemyState"));
        if (!Row) { return; }

        switch (NewState)
        {
        case EBXEnemyState::Patrol: MoveComp->MaxWalkSpeed = Row->WalkSpeed;   break;
        case EBXEnemyState::Alert:  MoveComp->MaxWalkSpeed = Row->AlertSpeed;  break;
        case EBXEnemyState::Combat: MoveComp->MaxWalkSpeed = Row->CombatSpeed; break;
        default: break;
        }
    }
}

void ABXEnemyCharacter::TakeBodyPartDamage(EBXBodyPart Part, float Damage)
{
    if (IsDead()) { return; }

    if (HealthBodyParts)
    {
        HealthBodyParts->ApplyDamageToBodyPart(Part, Damage);

        if (HealthBodyParts->IsDead())
        {
            Die();
        }
    }
}

float ABXEnemyCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    TakeBodyPartDamage(EBXBodyPart::Chest, DamageAmount);
    return DamageAmount;
}

void ABXEnemyCharacter::FireAtTarget(AActor* Target)
{
    if (!Target || IsDead() || !WeaponHandler) { return; }

    const float Now = GetWorld()->GetTimeSeconds();
    if (Now - LastFireTimeSec < AttackCooldownSec) { return; }
    LastFireTimeSec = Now;

    // 命中精度: BaseAccuracy で外れる場合は目標から少しずれた方向に発砲
    if (FMath::FRand() > BaseAccuracy)
    {
        return; // miss
    }

    const FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    SetActorRotation(ToTarget.Rotation());
    WeaponHandler->FirePrimary();
}

AActor* ABXEnemyCharacter::GetNextPatrolPoint()
{
    if (PatrolPoints.IsEmpty()) { return nullptr; }
    PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
    return PatrolPoints[PatrolIndex];
}

void ABXEnemyCharacter::Die()
{
    if (EnemyState == EBXEnemyState::Dead) { return; }
    EnemyState = EBXEnemyState::Dead;
    ApplyRagdoll();
    OnEnemyDied.Broadcast(this);
}

void ABXEnemyCharacter::ApplyRagdoll()
{
    if (USkeletalMeshComponent* SkelMesh = GetMesh())
    {
        SkelMesh->SetSimulatePhysics(true);
        SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
    if (UCapsuleComponent* Capsule = GetCapsuleComponent())
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->DisableMovement();
        MoveComp->StopMovementImmediately();
    }
}
