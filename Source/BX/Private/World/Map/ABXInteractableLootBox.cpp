// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.
#include "World/Map/ABXInteractableLootBox.h"
#include "Characters/APlayerCharacterBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"

ABXInteractableLootBox::ABXInteractableLootBox()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
    CollisionBox->SetCollisionProfileName(TEXT("OverlapAll"));
    RootComponent = CollisionBox;

    LootBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LootBoxMesh"));
    LootBoxMesh->SetupAttachment(RootComponent);
    LootBoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    OpenSound = CreateDefaultSubobject<UAudioComponent>(TEXT("OpenSound"));
    OpenSound->SetupAttachment(RootComponent);
    OpenSound->bAutoActivate = false;
}

void ABXInteractableLootBox::BeginPlay()
{
    Super::BeginPlay();
    GenerateLoot();
}

void ABXInteractableLootBox::OnInteract_Implementation(APlayerCharacterBase* Interactor)
{
    if (!CanInteract_Implementation(Interactor)) { return; }

    BoxState = GeneratedItems.IsEmpty() ? EBXLootBoxState::Empty : EBXLootBoxState::Open;

    OpenSound->Play();
    BP_OnBoxOpened(GeneratedItems);

    MarkAsEmpty();
}

FText ABXInteractableLootBox::GetInteractionPrompt_Implementation() const
{
    switch (BoxState)
    {
        case EBXLootBoxState::Closed: return FText::FromString(TEXT("開ける"));
        case EBXLootBoxState::Open:   return FText::FromString(TEXT("調べる"));
        case EBXLootBoxState::Empty:  return FText::FromString(TEXT("空"));
        default: return FText::GetEmpty();
    }
}

bool ABXInteractableLootBox::CanInteract_Implementation(APlayerCharacterBase* Interactor) const
{
    if (BoxState == EBXLootBoxState::Empty) { return false; }
    if (!Interactor) { return false; }
    return FVector::Dist(GetActorLocation(), Interactor->GetActorLocation()) <= InteractDistance;
}

void ABXInteractableLootBox::GenerateLoot()
{
    // TODO: DT_BX_LootTables からルート確率でアイテム生成
    // FBXLootTableRow を読んで確率ロール → GeneratedItems に追加
    // Sprint 28 ue5_scripts が DataTable を参照して CSV から初期化
    GeneratedItems.Reset();
}

void ABXInteractableLootBox::MarkAsEmpty()
{
    GeneratedItems.Reset();
    BoxState = EBXLootBoxState::Empty;
}
