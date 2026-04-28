// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

class APlayerCharacterBase;

#include "BXInteractableInterface.generated.h"

// UHT ルール: UINTERFACE の U/I プレフィックスは同じベース名でなければならない
// UBXInteractable → IBXInteractable (OK)
UINTERFACE(BlueprintType, MinimalAPI)
class UBXInteractable : public UInterface
{
    GENERATED_BODY()
};

// インタラクト可能オブジェクト共通インターフェース (SPEC §23-7)
// C++ 側は *_Implementation をオーバーライド。BP は BlueprintNativeEvent でオーバーライド可能。
class BX_API IBXInteractable
{
    GENERATED_BODY()

public:
    // インタラクト実行
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="BX|Interaction")
    void OnInteract(APlayerCharacterBase* Interactor);

    // UI 表示用プロンプト (例: 「拾う」「開ける」「調べる」)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="BX|Interaction")
    FText GetInteractionPrompt() const;

    // インタラクト可否チェック (距離・フラグ等を実装側で判定)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="BX|Interaction")
    bool CanInteract(APlayerCharacterBase* Interactor) const;
};
