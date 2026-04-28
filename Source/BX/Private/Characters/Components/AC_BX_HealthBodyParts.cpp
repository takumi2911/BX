// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Characters/Components/AC_BX_HealthBodyParts.h"

UAC_BX_HealthBodyParts::UAC_BX_HealthBodyParts()
{
    PrimaryComponentTick.bCanEverTick = false;

    // デフォルト最大 HP テーブル (SPEC §14-3)
    BodyPartMaxHP.Add(EBXBodyPart::Head,     35.0f);
    BodyPartMaxHP.Add(EBXBodyPart::Chest,   100.0f);
    BodyPartMaxHP.Add(EBXBodyPart::Abdomen,  70.0f);
    BodyPartMaxHP.Add(EBXBodyPart::LeftArm,  60.0f);
    BodyPartMaxHP.Add(EBXBodyPart::RightArm, 60.0f);
    BodyPartMaxHP.Add(EBXBodyPart::LeftLeg,  65.0f);
    BodyPartMaxHP.Add(EBXBodyPart::RightLeg, 65.0f);
}

void UAC_BX_HealthBodyParts::BeginPlay()
{
    Super::BeginPlay();

    // BodyPartMaxHP をベースに現在 HP を初期化
    for (const TPair<EBXBodyPart, float>& Pair : BodyPartMaxHP)
    {
        BodyPartHP.Add(Pair.Key, Pair.Value);
    }

    UE_LOG(LogTemp, Log, TEXT("UAC_BX_HealthBodyParts::BeginPlay — owner=%s, 7部位HP初期化完了"),
        GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"));
}

void UAC_BX_HealthBodyParts::ApplyDamageToBodyPart(EBXBodyPart Part, float Damage)
{
    if (bIsDead) { return; }

    float* HPPtr = BodyPartHP.Find(Part);
    if (!HPPtr) { return; }

    const float OldHP = *HPPtr;
    const float NewHP = FMath::Max(0.0f, OldHP - Damage);
    *HPPtr = NewHP;

    // 部位名文字列 (ログ用)
    const UEnum* PartEnum = StaticEnum<EBXBodyPart>();
    const FString PartName = PartEnum ? PartEnum->GetNameStringByValue(static_cast<int64>(Part)) : TEXT("?");

    UE_LOG(LogTemp, Warning, TEXT("BodyPartDamage: Part=%s HP=%.0f→%.0f (-%.1f) owner=%s"),
        *PartName, OldHP, NewHP, Damage,
        GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"));

    // 死亡判定 (Head=0 または Chest=0)
    const float HeadHP  = BodyPartHP.FindRef(EBXBodyPart::Head);
    const float ChestHP = BodyPartHP.FindRef(EBXBodyPart::Chest);
    if (!bIsDead && (HeadHP <= 0.0f || ChestHP <= 0.0f))
    {
        bIsDead = true;
        UE_LOG(LogTemp, Warning, TEXT("BodyPartDamage: IsDead=true (Head=%.0f Chest=%.0f) owner=%s"),
            HeadHP, ChestHP, GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"));
    }
}

float UAC_BX_HealthBodyParts::GetBodyPartHP(EBXBodyPart Part) const
{
    const float* HPPtr = BodyPartHP.Find(Part);
    return HPPtr ? *HPPtr : 0.0f;
}

float UAC_BX_HealthBodyParts::GetBodyPartHPRatio(EBXBodyPart Part) const
{
    const float* HPPtr    = BodyPartHP.Find(Part);
    const float* MaxHPPtr = BodyPartMaxHP.Find(Part);
    if (!HPPtr || !MaxHPPtr || *MaxHPPtr <= 0.0f) { return 0.0f; }
    return FMath::Clamp(*HPPtr / *MaxHPPtr, 0.0f, 1.0f);
}
