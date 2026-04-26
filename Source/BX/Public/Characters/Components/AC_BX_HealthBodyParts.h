// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AC_BX_HealthBodyParts.generated.h"

// TODO Sprint XX: 部位 HP 管理コンポーネント (SPEC §23-5)
// 現在はスタブ。APlayerCharacterBase が参照できるよう前方宣言代わりに配置。
UCLASS(ClassGroup=(BX), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class BX_API UAC_BX_HealthBodyParts : public UActorComponent
{
    GENERATED_BODY()
};
