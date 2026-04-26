// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AC_BX_WeaponHandler.generated.h"

// TODO Sprint 14: 武器ハンドラーコンポーネント (SPEC §23-6)
// 現在はスタブ。APlayerCharacterBase が参照できるよう前方宣言代わりに配置。
UCLASS(ClassGroup=(BX), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class BX_API UAC_BX_WeaponHandler : public UActorComponent
{
    GENERATED_BODY()
};
