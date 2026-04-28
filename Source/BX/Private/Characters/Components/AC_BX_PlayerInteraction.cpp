// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Characters/Components/AC_BX_PlayerInteraction.h"
#include "Characters/APlayerCharacterBase.h"
#include "Interaction/BXInteractableInterface.h"
#include "Camera/CameraComponent.h"

UAC_BX_PlayerInteraction::UAC_BX_PlayerInteraction()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAC_BX_PlayerInteraction::BeginPlay()
{
    Super::BeginPlay();
    OwnerPlayer = Cast<APlayerCharacterBase>(GetOwner());
    UE_LOG(LogTemp, Log, TEXT("UAC_BX_PlayerInteraction::BeginPlay — owner=%s"),
        OwnerPlayer.IsValid() ? *OwnerPlayer->GetName() : TEXT("NULL"));
}

void UAC_BX_PlayerInteraction::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    ScanForInteractable();
}

void UAC_BX_PlayerInteraction::ScanForInteractable()
{
    if (!OwnerPlayer.IsValid()) { return; }

    APlayerCharacterBase* Player = OwnerPlayer.Get();
    UCameraComponent* Cam = Player->CameraFirstPerson;
    if (!IsValid(Cam)) { return; }

    const FVector Start = Cam->GetComponentLocation();
    const FVector End   = Start + Cam->GetForwardVector() * InteractionRange;

    FHitResult Hit;
    FCollisionQueryParams Params(TEXT("InteractTrace"), false, Player);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    AActor* HitActor   = bHit ? Hit.GetActor() : nullptr;
    AActor* PrevFocused = CurrentFocusedActor.Get();

    if (IsValid(HitActor) && HitActor->Implements<UBXInteractable>())
    {
        if (IBXInteractable::Execute_CanInteract(HitActor, Player))
        {
            CurrentFocusedActor     = HitActor;
            CurrentPromptText       = IBXInteractable::Execute_GetInteractionPrompt(HitActor);
            bHasValidInteractTarget = true;
            FocusedTargetType       = EBXInteractTargetType::Generic; // TODO: 種別判定は後続 Sprint で拡張

            if (HitActor != PrevFocused)
            {
                UE_LOG(LogTemp, Log, TEXT("UAC_BX_PlayerInteraction: Focus → %s [%s]"),
                    *HitActor->GetName(), *CurrentPromptText.ToString());
                BP_OnFocusedTargetChanged(HitActor, FocusedTargetType);
            }
            return;
        }
    }

    // フォーカス外れ
    if (IsValid(PrevFocused))
    {
        UE_LOG(LogTemp, Log, TEXT("UAC_BX_PlayerInteraction: Focus cleared"));
        BP_OnFocusedTargetChanged(nullptr, EBXInteractTargetType::Generic);
    }
    CurrentFocusedActor     = nullptr;
    CurrentPromptText       = FText::GetEmpty();
    bHasValidInteractTarget = false;
    FocusedTargetType       = EBXInteractTargetType::Generic;
}

void UAC_BX_PlayerInteraction::TryInteract()
{
    UE_LOG(LogTemp, Warning, TEXT("UAC_BX_PlayerInteraction::TryInteract CALLED"));

    if (!bHasValidInteractTarget || !CurrentFocusedActor.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("TryInteract: no valid focused target"));
        return;
    }

    AActor* Target              = CurrentFocusedActor.Get();
    APlayerCharacterBase* Player = OwnerPlayer.Get();

    if (!IsValid(Target) || !IsValid(Player)) { return; }

    if (Target->Implements<UBXInteractable>())
    {
        UE_LOG(LogTemp, Warning, TEXT("TryInteract: OnInteract → %s"), *Target->GetName());
        IBXInteractable::Execute_OnInteract(Target, Player);
    }
}
