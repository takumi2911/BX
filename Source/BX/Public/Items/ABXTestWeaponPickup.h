// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/BXEnums.h"
#include "Interaction/BXInteractableInterface.h"

#include "ABXTestWeaponPickup.generated.h"

// テスト用地面武器ピックアップ (Sprint 14 / SPEC §23-5)
// IBXInteractable を実装し、E キーで WeaponHandlerComponent に EquipWeapon する。
// BP 派生 BP_TestWeaponPickup でメッシュ・WeaponRowName・TargetSlot を設定すること。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXTestWeaponPickup : public AActor, public IBXInteractable
{
    GENERATED_BODY()

public:
    ABXTestWeaponPickup();

    // 表示用メッシュ (BP_TestWeaponPickup で設定)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Pickup")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    // DT_BX_Weapons の Row Name (例: ar_556a_01)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Pickup")
    FName WeaponRowName = NAME_None;

    // 装備先スロット
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Pickup")
    EBXWeaponSlot TargetSlot = EBXWeaponSlot::Primary;

    // --- IBXInteractable ---
    virtual void OnInteract_Implementation(APlayerCharacterBase* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() const override;
    virtual bool CanInteract_Implementation(APlayerCharacterBase* Interactor) const override;
};
