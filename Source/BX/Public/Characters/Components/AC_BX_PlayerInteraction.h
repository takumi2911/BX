// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AC_BX_PlayerInteraction.generated.h"

// TODO Sprint 13: プレイヤーインタラクションコンポーネント
// 現在はスタブ。APlayerCharacterBase が参照できるよう前方宣言代わりに配置。
UCLASS(ClassGroup=(BX), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class BX_API UAC_BX_PlayerInteraction : public UActorComponent
{
    GENERATED_BODY()
};
