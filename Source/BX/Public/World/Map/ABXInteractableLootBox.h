// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/BXInteractableInterface.h"

#include "ABXInteractableLootBox.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UAudioComponent;
class UDataTable;

UENUM(BlueprintType)
enum class EBXLootBoxState : uint8
{
    Closed,
    Open,
    Empty,
};

// インタラクト可能なアイテム箱 (Sprint 28)
// IBXInteractable を実装。DT_BX_LootTables からルート生成する。
// スポーン時にルート確率でアイテム生成 → プレイヤーがインタラクトで取得。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXInteractableLootBox : public AActor, public IBXInteractable
{
    GENERATED_BODY()

public:
    ABXInteractableLootBox();

    virtual void BeginPlay() override;

    // --- IBXInteractable 実装 ---
    virtual void OnInteract_Implementation(APlayerCharacterBase* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() const override;
    virtual bool CanInteract_Implementation(APlayerCharacterBase* Interactor) const override;

    // --- コンポーネント ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UBoxComponent> CollisionBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UStaticMeshComponent> LootBoxMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UAudioComponent> OpenSound;

    // --- 設定 ---

    // LootTable を参照する DT_BX_LootTables の Row Name
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Loot")
    FName LootTableRowName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|Loot")
    TObjectPtr<UDataTable> LootTableDataTable;

    // 一度開けたら再起動まで再生成しない
    UPROPERTY(BlueprintReadOnly, Category="BX|Loot")
    EBXLootBoxState BoxState = EBXLootBoxState::Closed;

    // 生成されたアイテム (Row Name 一覧)
    UPROPERTY(BlueprintReadOnly, Category="BX|Loot")
    TArray<FName> GeneratedItems;

    // インタラクト距離 (cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Loot")
    float InteractDistance = 200.0f;

    // --- BlueprintImplementableEvent ---
    UFUNCTION(BlueprintImplementableEvent, Category="BX|Loot")
    void BP_OnBoxOpened(const TArray<FName>& ItemRowNames);

protected:
    void GenerateLoot();

private:
    void MarkAsEmpty();
};
