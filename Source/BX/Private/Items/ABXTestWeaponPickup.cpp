// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Items/ABXTestWeaponPickup.h"
#include "Characters/APlayerCharacterBase.h"
#include "Characters/Components/AC_BX_WeaponHandler.h"
#include "Components/StaticMeshComponent.h"

ABXTestWeaponPickup::ABXTestWeaponPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    SetRootComponent(MeshComponent);
}

void ABXTestWeaponPickup::OnInteract_Implementation(APlayerCharacterBase* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("ABXTestWeaponPickup::OnInteract — Row=%s Slot=%d"),
        *WeaponRowName.ToString(), static_cast<int32>(TargetSlot));

    if (!IsValid(Interactor))
    {
        UE_LOG(LogTemp, Error, TEXT("ABXTestWeaponPickup: Interactor is NULL"));
        return;
    }
    if (!IsValid(Interactor->WeaponHandlerComponent))
    {
        UE_LOG(LogTemp, Error, TEXT("ABXTestWeaponPickup: WeaponHandlerComponent is NULL on %s"),
            *Interactor->GetName());
        return;
    }
    if (WeaponRowName == NAME_None)
    {
        UE_LOG(LogTemp, Error,
            TEXT("ABXTestWeaponPickup: WeaponRowName is None — BP_TestWeaponPickup の Class Defaults で設定してください"));
        return;
    }

    Interactor->WeaponHandlerComponent->EquipWeapon(TargetSlot, WeaponRowName);
    UE_LOG(LogTemp, Warning, TEXT("ABXTestWeaponPickup: 武器装備完了 → Destroy"));
    Destroy();
}

FText ABXTestWeaponPickup::GetInteractionPrompt_Implementation() const
{
    return FText::FromString(TEXT("武器を拾う"));
}

bool ABXTestWeaponPickup::CanInteract_Implementation(APlayerCharacterBase* Interactor) const
{
    return WeaponRowName != NAME_None;
}
