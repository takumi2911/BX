// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/BXEnums.h"

class APlayerCharacterBase;

#include "AC_BX_PlayerInteraction.generated.h"

// プレイヤーインタラクションコンポーネント (SPEC §23-7)
// 視線前方 LineTrace でインタラクト対象(IBXInteractable)を検出し、TryInteract() で委譲する。
UCLASS(ClassGroup=(BX), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class BX_API UAC_BX_PlayerInteraction : public UActorComponent
{
    GENERATED_BODY()

public:
    UAC_BX_PlayerInteraction();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // インタラクト有効距離 (cm)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BX|Interaction", meta=(ClampMin="50.0", ClampMax="500.0"))
    float InteractionRange = 250.0f;

    // 現在フォーカスされている対象種別
    UPROPERTY(BlueprintReadOnly, Category="BX|Interaction")
    EBXInteractTargetType FocusedTargetType = EBXInteractTargetType::Generic;

    // 現在表示中のプロンプトテキスト
    UPROPERTY(BlueprintReadOnly, Category="BX|Interaction")
    FText CurrentPromptText;

    // 有効なインタラクト対象があるか
    UPROPERTY(BlueprintReadOnly, Category="BX|Interaction")
    bool bHasValidInteractTarget = false;

    // 現在フォーカスされている対象を返す (BP から参照用)
    UFUNCTION(BlueprintPure, Category="BX|Interaction")
    AActor* GetFocusedActor() const { return CurrentFocusedActor.Get(); }

    // IA_BX_Interact から呼ばれる — フォーカス対象に OnInteract を委譲する
    UFUNCTION(BlueprintCallable, Category="BX|Interaction")
    void TryInteract();

    // フォーカス対象変更通知 (BP でオーバーライド可)
    UFUNCTION(BlueprintImplementableEvent, Category="BX|Interaction")
    void BP_OnFocusedTargetChanged(AActor* NewFocusedActor, EBXInteractTargetType NewTargetType);

private:
    TWeakObjectPtr<APlayerCharacterBase> OwnerPlayer;
    TWeakObjectPtr<AActor> CurrentFocusedActor;

    // 毎フレーム LineTrace してインタラクト対象を走査する
    void ScanForInteractable();
};
