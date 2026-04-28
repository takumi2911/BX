// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

class UStaticMeshComponent;
class UAC_BX_HealthBodyParts;

#include "ABXTestDamageDummy.generated.h"

// 部位ダメージテスト用ダミー Actor (Sprint 15)
// SkeletalMesh 不在でも Cube StaticMesh で動作する。
// SkeletalMesh ありの場合は BoneName マッピングが有効になる。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXTestDamageDummy : public AActor
{
    GENERATED_BODY()

public:
    ABXTestDamageDummy();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UAC_BX_HealthBodyParts> HealthComponent;
};
