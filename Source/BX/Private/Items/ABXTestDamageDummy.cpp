// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Items/ABXTestDamageDummy.h"
#include "Characters/Components/AC_BX_HealthBodyParts.h"
#include "Components/StaticMeshComponent.h"

ABXTestDamageDummy::ABXTestDamageDummy()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    SetRootComponent(MeshComponent);

    HealthComponent = CreateDefaultSubobject<UAC_BX_HealthBodyParts>(TEXT("HealthComponent"));
}

void ABXTestDamageDummy::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("ABXTestDamageDummy: ダミー Actor 生成完了 — %s"), *GetName());
}
