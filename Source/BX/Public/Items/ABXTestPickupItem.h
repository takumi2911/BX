// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/BXInteractableInterface.h"

#include "ABXTestPickupItem.generated.h"

// テスト用地面ピックアップアイテム (Sprint 13 / SPEC §23-7)
// IBXInteractable を実装し、E キーで APlayerCharacterBase の InventoryComponent に AddItem する。
// BP 派生 BP_TestPickupItem でメッシュ・ItemRowName・Quantity を設定すること。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXTestPickupItem : public AActor, public IBXInteractable
{
    GENERATED_BODY()

public:
    ABXTestPickupItem();

    // 表示用メッシュ (BP_TestPickupItem で Engine/Cube 等をアサイン)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Pickup")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    // DT_BX_Items の Row Name (例: ammo_545x39_ps)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Pickup")
    FName ItemRowName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Pickup", meta=(ClampMin="1"))
    int32 Quantity = 1;

    // --- IBXInteractable ---
    virtual void OnInteract_Implementation(APlayerCharacterBase* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() const override;
    virtual bool CanInteract_Implementation(APlayerCharacterBase* Interactor) const override;
};
