// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Characters/Components/AC_BX_ArmorEquipment.h"
#include "Data/FBXArmorTableRow.h"
#include "Data/FBXArmorItemRow.h"
#include "Engine/DataTable.h"

UAC_BX_ArmorEquipment::UAC_BX_ArmorEquipment()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UAC_BX_ArmorEquipment::HasArmor(EBXBodyPart Part) const
{
    const FBXEquippedArmor* Armor = GetEquippedArmorForPart(Part);
    return Armor && Armor->ArmorRowName != NAME_None && Armor->CurrentDurability > 0.0f;
}

FBXArmorClassRow UAC_BX_ArmorEquipment::GetArmorClassRow(EBXBodyPart Part) const
{
    const FBXEquippedArmor* Armor = GetEquippedArmorForPart(Part);
    if (!Armor || Armor->ArmorRowName == NAME_None)
    {
        return FBXArmorClassRow{};
    }
    if (!IsValid(ArmorDataTable))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("UAC_BX_ArmorEquipment::GetArmorClassRow: ArmorDataTable が NULL — BP の Class Defaults で DT_BX_Armor をアサインしてください"));
        return FBXArmorClassRow{};
    }
    const FBXArmorClassRow* Row = ArmorDataTable->FindRow<FBXArmorClassRow>(
        Armor->ArmorRowName, TEXT("UAC_BX_ArmorEquipment::GetArmorClassRow"));
    if (!Row)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("UAC_BX_ArmorEquipment::GetArmorClassRow: RowName '%s' が DT_BX_Armor に見つかりません"),
            *Armor->ArmorRowName.ToString());
        return FBXArmorClassRow{};
    }
    return *Row;
}

float UAC_BX_ArmorEquipment::GetCurrentDurability(EBXBodyPart Part) const
{
    const FBXEquippedArmor* Armor = GetEquippedArmorForPart(Part);
    return Armor ? Armor->CurrentDurability : 0.0f;
}

float UAC_BX_ArmorEquipment::GetMaxDurability(EBXBodyPart Part) const
{
    const FBXEquippedArmor* Armor = GetEquippedArmorForPart(Part);
    return Armor ? Armor->MaxDurability : 100.0f;
}

void UAC_BX_ArmorEquipment::ApplyDurabilityDamage(EBXBodyPart Part, float DurabilityDamage)
{
    FBXEquippedArmor* Armor = GetEquippedArmorForPart(Part);
    if (!Armor) { return; }
    const float OldDur = Armor->CurrentDurability;
    Armor->CurrentDurability = FMath::Clamp(
        Armor->CurrentDurability - DurabilityDamage, 0.0f, Armor->MaxDurability);
    UE_LOG(LogTemp, Log,
        TEXT("ArmorDurability: Part=%d %.1f→%.1f/%.1f (-%.1f)"),
        static_cast<int32>(Part), OldDur, Armor->CurrentDurability, Armor->MaxDurability, DurabilityDamage);
}

void UAC_BX_ArmorEquipment::EquipArmorItem(FName ItemRowName)
{
    if (!IsValid(ArmorItemDataTable))
    {
        UE_LOG(LogTemp, Warning, TEXT("UAC_BX_ArmorEquipment::EquipArmorItem: ArmorItemDataTable が NULL"));
        return;
    }
    const FBXArmorItemRow* Item = ArmorItemDataTable->FindRow<FBXArmorItemRow>(
        ItemRowName, TEXT("UAC_BX_ArmorEquipment::EquipArmorItem"));
    if (!Item)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("UAC_BX_ArmorEquipment::EquipArmorItem: RowName '%s' が見つかりません"), *ItemRowName.ToString());
        return;
    }
    for (EBXBodyPart Part : Item->AreaCovered)
    {
        if (FBXEquippedArmor* ArmorSlot = GetEquippedArmorForPart(Part))
        {
            ArmorSlot->ArmorRowName     = Item->ArmorRowName;
            ArmorSlot->MaxDurability    = Item->MaxDurability;
            ArmorSlot->CurrentDurability = Item->MaxDurability;
        }
    }
    UE_LOG(LogTemp, Log,
        TEXT("UAC_BX_ArmorEquipment::EquipArmorItem: '%s' 装備完了 (AreaCovered: %d 部位)"),
        *ItemRowName.ToString(), Item->AreaCovered.Num());
}

FBXEquippedArmor* UAC_BX_ArmorEquipment::GetEquippedArmorForPart(EBXBodyPart Part)
{
    switch (Part)
    {
    case EBXBodyPart::Head:     return &HeadArmor;
    case EBXBodyPart::Chest:    return &ChestArmor;
    case EBXBodyPart::Abdomen:  return &AbdomenArmor;
    case EBXBodyPart::LeftArm:  return &LeftArmArmor;
    case EBXBodyPart::RightArm: return &RightArmArmor;
    case EBXBodyPart::LeftLeg:  return &LeftLegArmor;
    case EBXBodyPart::RightLeg: return &RightLegArmor;
    default:                    return nullptr;
    }
}

const FBXEquippedArmor* UAC_BX_ArmorEquipment::GetEquippedArmorForPart(EBXBodyPart Part) const
{
    switch (Part)
    {
    case EBXBodyPart::Head:     return &HeadArmor;
    case EBXBodyPart::Chest:    return &ChestArmor;
    case EBXBodyPart::Abdomen:  return &AbdomenArmor;
    case EBXBodyPart::LeftArm:  return &LeftArmArmor;
    case EBXBodyPart::RightArm: return &RightArmArmor;
    case EBXBodyPart::LeftLeg:  return &LeftLegArmor;
    case EBXBodyPart::RightLeg: return &RightLegArmor;
    default:                    return nullptr;
    }
}
