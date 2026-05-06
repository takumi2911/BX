// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Characters/APlayerCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Items/Components/AC_BX_Inventory.h"
#include "Characters/Components/AC_BX_StatusEffects.h"
#include "Characters/Components/AC_BX_HealthBodyParts.h"
#include "Characters/Components/AC_BX_WeaponHandler.h"
#include "Characters/Components/AC_BX_PlayerInteraction.h"
#include "UI/BXHUDWidget.h"
#include "Blueprint/UserWidget.h"

APlayerCharacterBase::APlayerCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // 一人称カメラ (先に作成して Mesh1P のアタッチ先にする)
    CameraFirstPerson = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFirstPerson"));
    CameraFirstPerson->SetupAttachment(GetRootComponent());
    CameraFirstPerson->SetRelativeLocation(FVector(-10.0f, 0.0f, 60.0f));
    CameraFirstPerson->bUsePawnControlRotation = true;

    // 一人称メッシュ (手元・武器用): CameraFirstPerson に直接アタッチ
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;
    Mesh1P->SetupAttachment(CameraFirstPerson);

    // SpringArm / 三人称カメラ (3P 視点用、デフォルト非アクティブ)
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->TargetArmLength = 300.0f;
    SpringArm->bUsePawnControlRotation = true;

    CameraThirdPerson = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraThirdPerson"));
    CameraThirdPerson->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    CameraThirdPerson->bUsePawnControlRotation = false;
    CameraThirdPerson->SetActive(false);

    // インベントリ / 状態異常コンポーネント
    InventoryComponent    = CreateDefaultSubobject<UAC_BX_Inventory>(TEXT("InventoryComponent"));
    StatusEffectsComponent = CreateDefaultSubobject<UAC_BX_StatusEffects>(TEXT("StatusEffectsComponent"));

    HealthBodyPartsComponent   = CreateDefaultSubobject<UAC_BX_HealthBodyParts>(TEXT("HealthBodyPartsComponent"));
    WeaponHandlerComponent     = CreateDefaultSubobject<UAC_BX_WeaponHandler>(TEXT("WeaponHandlerComponent"));
    PlayerInteractionComponent = CreateDefaultSubobject<UAC_BX_PlayerInteraction>(TEXT("PlayerInteractionComponent"));

    // 三人称メッシュ: 継承メッシュを BeginPlay で代入 (Mesh3P = GetMesh())
}

void APlayerCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    Mesh3P = GetMesh();
    InitializePlayerCharacter();

    // HUD Widget 作成 (BP_BX_Player の HUDWidgetClass に WBP_BX_HUD をアサインしてから有効)
    if (IsValid(HUDWidgetClass))
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            HUDWidgetInstance = CreateWidget<UBXHUDWidget>(PC, HUDWidgetClass);
            if (IsValid(HUDWidgetInstance))
            {
                HUDWidgetInstance->AddToViewport();
                HUDWidgetInstance->SetOwningPlayer(this);
                UE_LOG(LogTemp, Log, TEXT("BXHUDWidget: Created and added to viewport"));
            }
        }
    }
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsValid(HUDWidgetInstance)) { return; }

    // 部位 HP 更新 (7 部位)
    if (IsValid(HealthBodyPartsComponent))
    {
        static const EBXBodyPart AllParts[] = {
            EBXBodyPart::Head,
            EBXBodyPart::Chest,
            EBXBodyPart::Abdomen,
            EBXBodyPart::LeftArm,
            EBXBodyPart::RightArm,
            EBXBodyPart::LeftLeg,
            EBXBodyPart::RightLeg
        };
        for (EBXBodyPart Part : AllParts)
        {
            HUDWidgetInstance->OnUpdateBodyPartHP(Part, HealthBodyPartsComponent->GetBodyPartHPRatio(Part));
        }
    }

    // 弾薬更新
    if (IsValid(WeaponHandlerComponent))
    {
        const int32 CurrentAmmo = WeaponHandlerComponent->GetCurrentMagazineAmmo();
        const int32 MagSize     = WeaponHandlerComponent->GetMagazineCapacity();
        HUDWidgetInstance->OnUpdateAmmo(CurrentAmmo, MagSize);
        HUDWidgetInstance->OnUpdateCurrentWeapon(WeaponHandlerComponent->GetCurrentWeaponRowName());

        // HUD 公開フィールドも更新 (BP から直接参照される場合に備える)
        HudActiveWeaponAmmoCurrent = CurrentAmmo;
        HudActiveWeaponAmmoMax     = MagSize;
    }
}

void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UE_LOG(LogTemp, Warning, TEXT("APlayerCharacterBase::SetupPlayerInputComponent CALLED — %s"), *GetName());

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        UE_LOG(LogTemp, Warning, TEXT("EnhancedInputComponent cast OK"));
        BindAllInputActions(EIC);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("EnhancedInputComponent cast FAILED — PlayerInputComponent is %s"),
            PlayerInputComponent ? *PlayerInputComponent->GetClass()->GetName() : TEXT("NULL"));
    }
}

// =========================================================
// 主要関数
// =========================================================

void APlayerCharacterBase::InitializePlayerCharacter()
{
    UE_LOG(LogTemp, Log, TEXT("APlayerCharacterBase::InitializePlayerCharacter — %s"), *GetName());

    HudCarryMaxWeightKg = IsValid(InventoryComponent) ? InventoryComponent->MaxWeightKg : 24.0f;
}

void APlayerCharacterBase::BindAllInputActions(UEnhancedInputComponent* EIC)
{
    if (!IsValid(EIC)) { return; }

    UE_LOG(LogTemp, Warning, TEXT("APlayerCharacterBase::BindAllInputActions CALLED — %s"), *GetName());

    if (IsValid(IA_BX_Move))
    {
        EIC->BindAction(IA_BX_Move, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_MoveTriggered);
        UE_LOG(LogTemp, Warning, TEXT("IA_BX_Move BindAction OK"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("IA_BX_Move is NULL — BP_BX_Player の Class Defaults → BX|Input カテゴリで IA_BX_Move をアサインしてください"));
    }

    if (IsValid(IA_BX_Look))
    {
        EIC->BindAction(IA_BX_Look, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_LookTriggered);
        UE_LOG(LogTemp, Warning, TEXT("IA_BX_Look BindAction OK"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("IA_BX_Look is NULL — BP_BX_Player Class Defaults を確認してください"));
    }

    if (IsValid(IA_BX_Jump))
    {
        EIC->BindAction(IA_BX_Jump, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_JumpTriggered);
        UE_LOG(LogTemp, Warning, TEXT("IA_BX_Jump BindAction OK"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("IA_BX_Jump is NULL"));
    }

    if (IsValid(IA_BX_Sprint))
    {
        EIC->BindAction(IA_BX_Sprint, ETriggerEvent::Started,   this, &APlayerCharacterBase::Input_SprintStarted);
        EIC->BindAction(IA_BX_Sprint, ETriggerEvent::Completed, this, &APlayerCharacterBase::Input_SprintCompleted);
        UE_LOG(LogTemp, Warning, TEXT("IA_BX_Sprint BindAction OK"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("IA_BX_Sprint is NULL"));
    }

    if (IsValid(IA_BX_Crouch))
    {
        EIC->BindAction(IA_BX_Crouch, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_CrouchToggled);
        UE_LOG(LogTemp, Warning, TEXT("IA_BX_Crouch BindAction OK"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("IA_BX_Crouch is NULL"));
    }

    if (IsValid(IA_BX_Prone))
        EIC->BindAction(IA_BX_Prone, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_ProneToggled);

    if (IsValid(IA_BX_LeanLeft))
    {
        EIC->BindAction(IA_BX_LeanLeft, ETriggerEvent::Started,   this, &APlayerCharacterBase::Input_LeanLeftStarted);
        EIC->BindAction(IA_BX_LeanLeft, ETriggerEvent::Completed, this, &APlayerCharacterBase::Input_LeanLeftCompleted);
    }

    if (IsValid(IA_BX_LeanRight))
    {
        EIC->BindAction(IA_BX_LeanRight, ETriggerEvent::Started,   this, &APlayerCharacterBase::Input_LeanRightStarted);
        EIC->BindAction(IA_BX_LeanRight, ETriggerEvent::Completed, this, &APlayerCharacterBase::Input_LeanRightCompleted);
    }

    if (IsValid(IA_BX_FirePrimary))
    {
        EIC->BindAction(IA_BX_FirePrimary, ETriggerEvent::Started,   this, &APlayerCharacterBase::Input_FirePrimaryStarted);
        EIC->BindAction(IA_BX_FirePrimary, ETriggerEvent::Completed, this, &APlayerCharacterBase::Input_FirePrimaryCompleted);
    }

    if (IsValid(IA_BX_FireSecondary))
        EIC->BindAction(IA_BX_FireSecondary, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_FireSecondaryTriggered);

    if (IsValid(IA_BX_Reload))
        EIC->BindAction(IA_BX_Reload, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_ReloadTriggered);

    if (IsValid(IA_BX_SwitchWeaponPrimary))
        EIC->BindAction(IA_BX_SwitchWeaponPrimary, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_SwitchWeaponSlot, EBXWeaponSlot::Primary);

    if (IsValid(IA_BX_SwitchWeaponSecondary))
        EIC->BindAction(IA_BX_SwitchWeaponSecondary, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_SwitchWeaponSlot, EBXWeaponSlot::Secondary);

    if (IsValid(IA_BX_SwitchWeaponPistol))
        EIC->BindAction(IA_BX_SwitchWeaponPistol, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_SwitchWeaponSlot, EBXWeaponSlot::Pistol);

    if (IsValid(IA_BX_SwitchWeaponMelee))
        EIC->BindAction(IA_BX_SwitchWeaponMelee, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_SwitchWeaponSlot, EBXWeaponSlot::Melee);

    if (IsValid(IA_BX_Holster))
        EIC->BindAction(IA_BX_Holster, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_HolsterTriggered);

    if (IsValid(IA_BX_Interact))
        EIC->BindAction(IA_BX_Interact, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_InteractTriggered);

    if (IsValid(IA_BX_MedicalUse))
        EIC->BindAction(IA_BX_MedicalUse, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_MedicalUseTriggered);

    if (IsValid(IA_BX_QuickSlot1))
        EIC->BindAction(IA_BX_QuickSlot1, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_QuickSlotTriggered, static_cast<int32>(0));

    if (IsValid(IA_BX_QuickSlot2))
        EIC->BindAction(IA_BX_QuickSlot2, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_QuickSlotTriggered, static_cast<int32>(1));

    if (IsValid(IA_BX_QuickSlot3))
        EIC->BindAction(IA_BX_QuickSlot3, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_QuickSlotTriggered, static_cast<int32>(2));

    if (IsValid(IA_BX_QuickSlot4))
        EIC->BindAction(IA_BX_QuickSlot4, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_QuickSlotTriggered, static_cast<int32>(3));

    if (IsValid(IA_BX_OpenInventory))
        EIC->BindAction(IA_BX_OpenInventory, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_OpenInventoryTriggered);

    if (IsValid(IA_BX_OpenMap))
        EIC->BindAction(IA_BX_OpenMap, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Input_OpenMapTriggered);
}

void APlayerCharacterBase::SetViewMode(EBXViewMode NewViewMode)
{
    if (CurrentViewMode == NewViewMode) { return; }
    CurrentViewMode = NewViewMode;

    const bool bFirst = (NewViewMode == EBXViewMode::First);
    if (IsValid(CameraFirstPerson))  { CameraFirstPerson->SetActive(bFirst); }
    if (IsValid(CameraThirdPerson))  { CameraThirdPerson->SetActive(!bFirst); }
    if (IsValid(Mesh1P))             { Mesh1P->SetVisibility(bFirst); }

    BP_OnViewModeChanged(NewViewMode);
}

void APlayerCharacterBase::LockInput(FGameplayTag LockTag)
{
    MovementInputLockTags.AddTag(LockTag);
    bInputLocked = !MovementInputLockTags.IsEmpty() || !CombatInputLockTags.IsEmpty();
}

void APlayerCharacterBase::UnlockInput(FGameplayTag LockTag)
{
    MovementInputLockTags.RemoveTag(LockTag);
    CombatInputLockTags.RemoveTag(LockTag);
    bInputLocked = !MovementInputLockTags.IsEmpty() || !CombatInputLockTags.IsEmpty();
}

void APlayerCharacterBase::NotifyDeathOrUnconscious()
{
    bAlive = false;
    bInputLocked = true;
    BP_OnPlayerDeath();
    UE_LOG(LogTemp, Warning, TEXT("APlayerCharacterBase::NotifyDeathOrUnconscious — %s"), *GetName());
}

void APlayerCharacterBase::ExportPlayerSaveState(FBXPlayerSaveState& OutState) const
{
    OutState.SavedTransform = GetActorTransform();
    OutState.SavedViewMode  = CurrentViewMode;
    OutState.SavedHealthState.bIsAlive       = bAlive;
    OutState.SavedHealthState.bIsUnconscious = bUnconscious;

    if (IsValid(InventoryComponent))
    {
        OutState.SavedInventoryState.TotalWeightKg = InventoryComponent->GetTotalWeightKg();
    }
    UE_LOG(LogTemp, Log, TEXT("APlayerCharacterBase::ExportPlayerSaveState — %s"), *GetName());
}

void APlayerCharacterBase::ImportPlayerSaveState(const FBXPlayerSaveState& InState)
{
    SetActorTransform(InState.SavedTransform);
    SetViewMode(InState.SavedViewMode);
    bAlive       = InState.SavedHealthState.bIsAlive;
    bUnconscious = InState.SavedHealthState.bIsUnconscious;
    UE_LOG(LogTemp, Log, TEXT("APlayerCharacterBase::ImportPlayerSaveState — %s"), *GetName());
}

// =========================================================
// 入力ハンドラ スタブ
// =========================================================

void APlayerCharacterBase::Input_MoveTriggered(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Warning, TEXT("Input_MoveTriggered CALLED"));

    if (bInputLocked) { return; }
    const FVector2D MoveVec = Value.Get<FVector2D>();
    AddMovementInput(GetActorForwardVector(), MoveVec.Y);
    AddMovementInput(GetActorRightVector(),   MoveVec.X);

    if (CurrentLocomotionState == EBXLocomotionState::Idle)
    {
        CurrentLocomotionState = EBXLocomotionState::Walk;
        BP_OnLocomotionStateChanged(CurrentLocomotionState);
    }
}

void APlayerCharacterBase::Input_LookTriggered(const FInputActionValue& Value)
{
    const FVector2D LookVec = Value.Get<FVector2D>();
    AddControllerYawInput(LookVec.X);
    AddControllerPitchInput(LookVec.Y);
}

void APlayerCharacterBase::Input_JumpTriggered()
{
    if (bInputLocked) { return; }
    Jump();
    UE_LOG(LogTemp, Verbose, TEXT("Input_JumpTriggered"));
}

void APlayerCharacterBase::Input_SprintStarted()
{
    if (bInputLocked) { return; }
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    CurrentLocomotionState = EBXLocomotionState::Sprint;
    BP_OnLocomotionStateChanged(CurrentLocomotionState);
    UE_LOG(LogTemp, Verbose, TEXT("Input_SprintStarted"));
}

void APlayerCharacterBase::Input_SprintCompleted()
{
    GetCharacterMovement()->MaxWalkSpeed = 400.0f;
    if (CurrentLocomotionState == EBXLocomotionState::Sprint)
    {
        CurrentLocomotionState = EBXLocomotionState::Run;
        BP_OnLocomotionStateChanged(CurrentLocomotionState);
    }
    UE_LOG(LogTemp, Verbose, TEXT("Input_SprintCompleted"));
}

void APlayerCharacterBase::Input_CrouchToggled()
{
    if (bInputLocked) { return; }
    if (CurrentLocomotionState == EBXLocomotionState::Crouch)
    {
        UnCrouch();
        CurrentLocomotionState = EBXLocomotionState::Idle;
    }
    else
    {
        Crouch();
        CurrentLocomotionState = EBXLocomotionState::Crouch;
    }
    BP_OnLocomotionStateChanged(CurrentLocomotionState);
    UE_LOG(LogTemp, Verbose, TEXT("Input_CrouchToggled"));
}

void APlayerCharacterBase::Input_ProneToggled()
{
    if (bInputLocked) { return; }
    const bool bGoProne = (CurrentLocomotionState != EBXLocomotionState::Prone);
    CurrentLocomotionState = bGoProne ? EBXLocomotionState::Prone : EBXLocomotionState::Idle;
    BP_OnLocomotionStateChanged(CurrentLocomotionState);
    UE_LOG(LogTemp, Verbose, TEXT("Input_ProneToggled: %s"), bGoProne ? TEXT("Prone") : TEXT("Stand"));
}

void APlayerCharacterBase::Input_LeanLeftStarted()
{
    if (bInputLocked) { return; }
    CurrentLeanState = EBXLeanState::Left;
    UE_LOG(LogTemp, Verbose, TEXT("Input_LeanLeftStarted"));
}

void APlayerCharacterBase::Input_LeanLeftCompleted()
{
    if (CurrentLeanState == EBXLeanState::Left) { CurrentLeanState = EBXLeanState::None; }
    UE_LOG(LogTemp, Verbose, TEXT("Input_LeanLeftCompleted"));
}

void APlayerCharacterBase::Input_LeanRightStarted()
{
    if (bInputLocked) { return; }
    CurrentLeanState = EBXLeanState::Right;
    UE_LOG(LogTemp, Verbose, TEXT("Input_LeanRightStarted"));
}

void APlayerCharacterBase::Input_LeanRightCompleted()
{
    if (CurrentLeanState == EBXLeanState::Right) { CurrentLeanState = EBXLeanState::None; }
    UE_LOG(LogTemp, Verbose, TEXT("Input_LeanRightCompleted"));
}

void APlayerCharacterBase::Input_FirePrimaryStarted()
{
    UE_LOG(LogTemp, Warning, TEXT("Input_FirePrimaryStarted CALLED"));
    if (bInputLocked) { return; }
    CurrentCombatState = EBXCombatState::Firing;
    BP_OnCombatStateChanged(CurrentCombatState);

    if (IsValid(WeaponHandlerComponent))
    {
        WeaponHandlerComponent->FirePrimary();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Input_FirePrimaryStarted: WeaponHandlerComponent is NULL"));
    }
}

void APlayerCharacterBase::Input_FirePrimaryCompleted()
{
    if (CurrentCombatState == EBXCombatState::Firing)
    {
        CurrentCombatState = EBXCombatState::None;
        BP_OnCombatStateChanged(CurrentCombatState);
    }
    UE_LOG(LogTemp, Verbose, TEXT("Input_FirePrimaryCompleted"));
}

void APlayerCharacterBase::Input_FireSecondaryTriggered()
{
    if (bInputLocked) { return; }
    const bool bWasAiming = (CurrentCombatState == EBXCombatState::Aiming);
    CurrentCombatState = bWasAiming ? EBXCombatState::None : EBXCombatState::Aiming;
    BP_OnCombatStateChanged(CurrentCombatState);
    UE_LOG(LogTemp, Verbose, TEXT("Input_FireSecondaryTriggered (ADS toggle)"));
}

void APlayerCharacterBase::Input_ReloadTriggered()
{
    UE_LOG(LogTemp, Warning, TEXT("Input_ReloadTriggered CALLED"));
    if (bInputLocked) { return; }
    CurrentCombatState = EBXCombatState::Reloading;
    BP_OnCombatStateChanged(CurrentCombatState);

    if (IsValid(WeaponHandlerComponent))
    {
        WeaponHandlerComponent->Reload();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Input_ReloadTriggered: WeaponHandlerComponent is NULL"));
    }
}

void APlayerCharacterBase::Input_SwitchWeaponSlot(EBXWeaponSlot WeaponSlot)
{
    UE_LOG(LogTemp, Warning, TEXT("Input_SwitchWeaponSlot CALLED — slot=%d"), static_cast<int32>(WeaponSlot));
    if (bInputLocked) { return; }
    CurrentCombatState = EBXCombatState::Switching;
    BP_OnCombatStateChanged(CurrentCombatState);

    if (IsValid(WeaponHandlerComponent))
    {
        WeaponHandlerComponent->SwitchWeaponSlot(WeaponSlot);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Input_SwitchWeaponSlot: WeaponHandlerComponent is NULL"));
    }
}

void APlayerCharacterBase::Input_HolsterTriggered()
{
    if (bInputLocked) { return; }
    UE_LOG(LogTemp, Log, TEXT("Input_HolsterTriggered — TODO: WeaponHandlerComponent (Sprint 14)"));
}

void APlayerCharacterBase::Input_InteractTriggered()
{
    UE_LOG(LogTemp, Warning, TEXT("Input_InteractTriggered CALLED"));

    if (bInputLocked) { return; }
    bIsInteractionActive = true;

    if (IsValid(PlayerInteractionComponent))
    {
        PlayerInteractionComponent->TryInteract();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Input_InteractTriggered: PlayerInteractionComponent is NULL"));
    }
}

void APlayerCharacterBase::Input_MedicalUseTriggered()
{
    if (bInputLocked) { return; }
    bIsMedicalUseActive = true;
    UE_LOG(LogTemp, Log, TEXT("Input_MedicalUseTriggered — TODO: UBXMedicalUseSubsystem 連携"));
}

void APlayerCharacterBase::Input_QuickSlotTriggered(int32 QuickSlotIndex)
{
    if (bInputLocked) { return; }
    UE_LOG(LogTemp, Log, TEXT("Input_QuickSlotTriggered: slot=%d"), QuickSlotIndex);
}

void APlayerCharacterBase::Input_OpenInventoryTriggered()
{
    UE_LOG(LogTemp, Log, TEXT("Input_OpenInventoryTriggered — TODO: UI Subsystem 連携"));
}

void APlayerCharacterBase::Input_OpenMapTriggered()
{
    UE_LOG(LogTemp, Log, TEXT("Input_OpenMapTriggered — TODO: UI Subsystem 連携"));
}
