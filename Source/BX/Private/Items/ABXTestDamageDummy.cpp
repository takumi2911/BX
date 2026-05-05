// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Items/ABXTestDamageDummy.h"
#include "Characters/Components/AC_BX_HealthBodyParts.h"
#include "Characters/Components/AC_BX_ArmorEquipment.h"
#include "Components/StaticMeshComponent.h"

ABXTestDamageDummy::ABXTestDamageDummy()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    SetRootComponent(MeshComponent);

    HealthComponent = CreateDefaultSubobject<UAC_BX_HealthBodyParts>(TEXT("HealthComponent"));

    ArmorComponent = CreateDefaultSubobject<UAC_BX_ArmorEquipment>(TEXT("ArmorComponent"));
    // デフォルト: Chest に ArmorClass=3 (BasePenThresh=28) を装備
    ArmorComponent->ChestArmor.ArmorRowName = FName("armor_class_3");
    ArmorComponent->ChestArmor.CurrentDurability = 100.0f;
    ArmorComponent->ChestArmor.MaxDurability = 100.0f;
}

void ABXTestDamageDummy::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("ABXTestDamageDummy: ダミー Actor 生成完了 — %s"), *GetName());
}
