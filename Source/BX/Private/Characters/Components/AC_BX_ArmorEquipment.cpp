// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Characters/Components/AC_BX_ArmorEquipment.h"
#include "Data/FBXArmorTableRow.h"
#include "Engine/DataTable.h"

UAC_BX_ArmorEquipment::UAC_BX_ArmorEquipment()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UAC_BX_ArmorEquipment::HasArmor(EBXBodyPart Part) const
{
    // Sprint 17: Chest のみ防具判定。Sprint 18 で他部位を追加。
    if (Part != EBXBodyPart::Chest) { return false; }
    const FBXEquippedArmor* Armor = GetEquippedArmorForPart(Part);
    return Armor && Armor->ArmorRowName != NAME_None && Armor->CurrentDurability > 0.0f;
}

FBXArmorClassRow UAC_BX_ArmorEquipment::GetArmorClassRow(EBXBodyPart Part) const
{
    const FBXEquippedArmor* Armor = GetEquippedArmorForPart(Part);
    if (!Armor || Armor->ArmorRowName == NAME_None)
    {
        UE_LOG(LogTemp, Warning, TEXT("UAC_BX_ArmorEquipment::GetArmorClassRow: 部位 %d に防具なし"), static_cast<int32>(Part));
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
    Armor->CurrentDurability = FMath::Clamp(Armor->CurrentDurability - DurabilityDamage, 0.0f, Armor->MaxDurability);
    UE_LOG(LogTemp, Log, TEXT("ArmorDurability: Part=%d Durability=%.1f→%.1f/%.1f (-%.1f)"),
        static_cast<int32>(Part), OldDur, Armor->CurrentDurability, Armor->MaxDurability, DurabilityDamage);
}

FBXEquippedArmor* UAC_BX_ArmorEquipment::GetEquippedArmorForPart(EBXBodyPart Part)
{
    if (Part == EBXBodyPart::Chest) { return &ChestArmor; }
    return nullptr;
}

const FBXEquippedArmor* UAC_BX_ArmorEquipment::GetEquippedArmorForPart(EBXBodyPart Part) const
{
    if (Part == EBXBodyPart::Chest) { return &ChestArmor; }
    return nullptr;
}
