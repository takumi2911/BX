// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Data/BXBodyPartHelpers.h"

EBXBodyPart FBXBodyPartHelpers::BoneNameToBodyPart(FName BoneName)
{
    if (BoneName == NAME_None)
    {
        return EBXBodyPart::Chest;
    }

    const FString Lower = BoneName.ToString().ToLower();

    // 頭部
    if (Lower.Contains(TEXT("head")) || Lower.Contains(TEXT("neck")))
    {
        UE_LOG(LogTemp, Verbose, TEXT("BoneNameToBodyPart: BoneName=%s → Head"), *BoneName.ToString());
        return EBXBodyPart::Head;
    }

    // 胸部 (spine_03 / clavicle は上半身 = Chest)
    if (Lower.Contains(TEXT("spine_03")) || Lower.Contains(TEXT("clavicle")) ||
        Lower.Contains(TEXT("spine_04")) || Lower.Contains(TEXT("spine_05")))
    {
        UE_LOG(LogTemp, Verbose, TEXT("BoneNameToBodyPart: BoneName=%s → Chest"), *BoneName.ToString());
        return EBXBodyPart::Chest;
    }

    // 腹部 (spine_01 / spine_02 / pelvis は下半身 = Abdomen)
    if (Lower.Contains(TEXT("spine_01")) || Lower.Contains(TEXT("spine_02")) ||
        Lower.Contains(TEXT("pelvis")))
    {
        UE_LOG(LogTemp, Verbose, TEXT("BoneNameToBodyPart: BoneName=%s → Abdomen"), *BoneName.ToString());
        return EBXBodyPart::Abdomen;
    }

    // 左腕
    if (Lower.Contains(TEXT("upperarm_l")) || Lower.Contains(TEXT("lowerarm_l")) ||
        Lower.Contains(TEXT("hand_l")) || Lower.Contains(TEXT("index_l")) ||
        Lower.Contains(TEXT("middle_l")) || Lower.Contains(TEXT("ring_l")) ||
        Lower.Contains(TEXT("pinky_l")) || Lower.Contains(TEXT("thumb_l")))
    {
        UE_LOG(LogTemp, Verbose, TEXT("BoneNameToBodyPart: BoneName=%s → LeftArm"), *BoneName.ToString());
        return EBXBodyPart::LeftArm;
    }

    // 右腕
    if (Lower.Contains(TEXT("upperarm_r")) || Lower.Contains(TEXT("lowerarm_r")) ||
        Lower.Contains(TEXT("hand_r")) || Lower.Contains(TEXT("index_r")) ||
        Lower.Contains(TEXT("middle_r")) || Lower.Contains(TEXT("ring_r")) ||
        Lower.Contains(TEXT("pinky_r")) || Lower.Contains(TEXT("thumb_r")))
    {
        UE_LOG(LogTemp, Verbose, TEXT("BoneNameToBodyPart: BoneName=%s → RightArm"), *BoneName.ToString());
        return EBXBodyPart::RightArm;
    }

    // 左脚
    if (Lower.Contains(TEXT("thigh_l")) || Lower.Contains(TEXT("calf_l")) ||
        Lower.Contains(TEXT("foot_l")) || Lower.Contains(TEXT("ball_l")))
    {
        UE_LOG(LogTemp, Verbose, TEXT("BoneNameToBodyPart: BoneName=%s → LeftLeg"), *BoneName.ToString());
        return EBXBodyPart::LeftLeg;
    }

    // 右脚
    if (Lower.Contains(TEXT("thigh_r")) || Lower.Contains(TEXT("calf_r")) ||
        Lower.Contains(TEXT("foot_r")) || Lower.Contains(TEXT("ball_r")))
    {
        UE_LOG(LogTemp, Verbose, TEXT("BoneNameToBodyPart: BoneName=%s → RightLeg"), *BoneName.ToString());
        return EBXBodyPart::RightLeg;
    }

    // 不明ボーンはデフォルト Chest にマップ
    UE_LOG(LogTemp, Verbose, TEXT("BoneNameToBodyPart: BoneName=%s → Chest (デフォルト)"), *BoneName.ToString());
    return EBXBodyPart::Chest;
}
