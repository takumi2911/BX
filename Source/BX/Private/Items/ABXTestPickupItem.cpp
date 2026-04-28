// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Items/ABXTestPickupItem.h"
#include "Characters/APlayerCharacterBase.h"
#include "Items/Components/AC_BX_Inventory.h"
#include "Components/StaticMeshComponent.h"

ABXTestPickupItem::ABXTestPickupItem()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    SetRootComponent(MeshComponent);
}

void ABXTestPickupItem::OnInteract_Implementation(APlayerCharacterBase* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("ABXTestPickupItem::OnInteract — Item=%s Qty=%d"), *ItemRowName.ToString(), Quantity);

    if (!IsValid(Interactor))
    {
        UE_LOG(LogTemp, Error, TEXT("ABXTestPickupItem: Interactor is NULL"));
        return;
    }
    if (!IsValid(Interactor->InventoryComponent))
    {
        UE_LOG(LogTemp, Error, TEXT("ABXTestPickupItem: InventoryComponent is NULL on %s"), *Interactor->GetName());
        return;
    }
    if (ItemRowName == NAME_None)
    {
        UE_LOG(LogTemp, Error, TEXT("ABXTestPickupItem: ItemRowName is None — BP_TestPickupItem の Class Defaults で設定してください"));
        return;
    }

    const bool bAdded = Interactor->InventoryComponent->AddItem(ItemRowName, Quantity, /*bFoundInRaid=*/true);
    if (bAdded)
    {
        UE_LOG(LogTemp, Warning, TEXT("ABXTestPickupItem: Pickup 成功 → %s x%d"), *ItemRowName.ToString(), Quantity);
        Destroy();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ABXTestPickupItem: Pickup 失敗 (重量超過 or スタック上限) → %s"), *ItemRowName.ToString());
    }
}

FText ABXTestPickupItem::GetInteractionPrompt_Implementation() const
{
    return FText::FromString(TEXT("拾う"));
}

bool ABXTestPickupItem::CanInteract_Implementation(APlayerCharacterBase* Interactor) const
{
    return ItemRowName != NAME_None;
}
