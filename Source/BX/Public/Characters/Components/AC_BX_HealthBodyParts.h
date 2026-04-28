// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/BXEnums.h"

#include "AC_BX_HealthBodyParts.generated.h"

// 部位別 HP 管理コンポーネント (SPEC §14-3)
// 7部位 HP を保持し ApplyDamageToBodyPart で部位ダメージを受け付ける。
// 死亡処理(Ragdoll/リスポーン)は Sprint 18 で実装。
UCLASS(ClassGroup=(BX), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class BX_API UAC_BX_HealthBodyParts : public UActorComponent
{
    GENERATED_BODY()

public:
    UAC_BX_HealthBodyParts();

protected:
    virtual void BeginPlay() override;

public:
    // =========================================================
    // 部位 HP データ
    // =========================================================

    UPROPERTY(BlueprintReadOnly, Category="BX|BodyParts")
    TMap<EBXBodyPart, float> BodyPartHP;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BX|BodyParts")
    TMap<EBXBodyPart, float> BodyPartMaxHP;

    UPROPERTY(BlueprintReadOnly, Category="BX|BodyParts")
    bool bIsDead = false;

    // =========================================================
    // 主要関数
    // =========================================================

    // 部位にダメージを与える。HP が 0 以下にならないよう clamp。
    UFUNCTION(BlueprintCallable, Category="BX|BodyParts")
    void ApplyDamageToBodyPart(EBXBodyPart Part, float Damage);

    UFUNCTION(BlueprintPure, Category="BX|BodyParts")
    float GetBodyPartHP(EBXBodyPart Part) const;

    // 現在 HP / 最大 HP の割合 (0.0〜1.0)
    UFUNCTION(BlueprintPure, Category="BX|BodyParts")
    float GetBodyPartHPRatio(EBXBodyPart Part) const;

    // Head または Chest が 0 なら true (死亡判定のみ、処理は Sprint 18)
    UFUNCTION(BlueprintPure, Category="BX|BodyParts")
    bool IsDead() const { return bIsDead; }
};
