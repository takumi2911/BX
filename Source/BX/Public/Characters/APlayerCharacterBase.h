// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "Data/BXEnums.h"
#include "Save/FBXPlayerSaveState.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UEnhancedInputComponent;
class UAC_BX_Inventory;
class UAC_BX_StatusEffects;
class UAC_BX_HealthBodyParts;
class UAC_BX_WeaponHandler;
class UAC_BX_PlayerInteraction;

#include "APlayerCharacterBase.generated.h"

// プレイヤーキャラクター基底クラス (SPEC §23-4)
// 入力受付 / カメラ管理 / 各コンポーネント統合 / Save 連携を担う。
// 個別ロジックはコンポーネント側が持つ。BP 派生: BP_BX_Player
UCLASS(BlueprintType, Blueprintable, Abstract)
class BX_API APlayerCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacterBase();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // =========================================================
    // コンポーネント
    // =========================================================

    // 一人称アームズメッシュ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<USkeletalMeshComponent> Mesh1P;

    // 三人称メッシュ (BeginPlay で ACharacter::GetMesh() を参照)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<USkeletalMeshComponent> Mesh3P;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UCameraComponent> CameraFirstPerson;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<USpringArmComponent> SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UCameraComponent> CameraThirdPerson;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UAC_BX_Inventory> InventoryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UAC_BX_StatusEffects> StatusEffectsComponent;

    // TODO Sprint XX: UAC_BX_HealthBodyParts 実装後に CreateDefaultSubobject を追加
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UAC_BX_HealthBodyParts> HealthBodyPartsComponent;

    // TODO Sprint 14: UAC_BX_WeaponHandler 実装後に CreateDefaultSubobject を追加
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UAC_BX_WeaponHandler> WeaponHandlerComponent;

    // TODO Sprint 13: UAC_BX_PlayerInteraction 実装後に CreateDefaultSubobject を追加
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UAC_BX_PlayerInteraction> PlayerInteractionComponent;

    // =========================================================
    // 入力アクション (アセットは BP_BX_Player で割り当て)
    // =========================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_Move;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_Look;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_Jump;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_Sprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_Crouch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_Prone;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_LeanLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_LeanRight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_FirePrimary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_FireSecondary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_Reload;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_SwitchWeaponPrimary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_SwitchWeaponSecondary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_SwitchWeaponPistol;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_SwitchWeaponMelee;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_Holster;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_Interact;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_MedicalUse;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_QuickSlot1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_QuickSlot2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_QuickSlot3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_QuickSlot4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_OpenInventory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Input")
    TObjectPtr<UInputAction> IA_BX_OpenMap;

    // =========================================================
    // 視点・姿勢・移動・戦闘状態 (SPEC §23-4)
    // =========================================================

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    EBXViewMode CurrentViewMode = EBXViewMode::First;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    EBXLocomotionState CurrentLocomotionState = EBXLocomotionState::Idle;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    EBXLeanState CurrentLeanState = EBXLeanState::None;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    EBXCombatState CurrentCombatState = EBXCombatState::None;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    bool bIsMedicalUseActive = false;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    bool bIsInteractionActive = false;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    bool bIsExtractingActive = false;

    // =========================================================
    // 状態フラグ (SPEC §23-4)
    // =========================================================

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    bool bAlive = true;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    bool bUnconscious = false;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    bool bIsInExtractZone = false;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    bool bInputLocked = false;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    FGameplayTagContainer MovementInputLockTags;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player")
    FGameplayTagContainer CombatInputLockTags;

    // =========================================================
    // HUD 公開フィールド (SPEC §23-4)
    // =========================================================

    UPROPERTY(BlueprintReadOnly, Category="BX|Player|HUD")
    float HudCarryWeightKg = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player|HUD")
    float HudCarryMaxWeightKg = 24.0f;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player|HUD")
    EBXCarryState HudCarryState = EBXCarryState::Normal;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player|HUD")
    EBXLanguage HudCurrentLanguage = EBXLanguage::Japanese;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player|HUD")
    FText HudActiveWeaponName;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player|HUD")
    int32 HudActiveWeaponAmmoCurrent = 0;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player|HUD")
    int32 HudActiveWeaponAmmoMax = 0;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player|HUD")
    FText HudCurrentExtractName;

    UPROPERTY(BlueprintReadOnly, Category="BX|Player|HUD")
    float HudCurrentExtractRemainingSec = 0.0f;

    // =========================================================
    // 主要関数 (SPEC §23-4)
    // =========================================================

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void InitializePlayerCharacter();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void BindAllInputActions(UEnhancedInputComponent* EIC);

    // --- 視点 ---

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void SetViewMode(EBXViewMode NewViewMode);

    UFUNCTION(BlueprintPure, Category="BX|Player")
    EBXViewMode GetCurrentViewMode() const { return CurrentViewMode; }

    UFUNCTION(BlueprintPure, Category="BX|Player")
    EBXLocomotionState GetCurrentLocomotionState() const { return CurrentLocomotionState; }

    UFUNCTION(BlueprintPure, Category="BX|Player")
    EBXCombatState GetCurrentCombatState() const { return CurrentCombatState; }

    // --- 入力ロック ---

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void LockInput(FGameplayTag LockTag);

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void UnlockInput(FGameplayTag LockTag);

    UFUNCTION(BlueprintPure, Category="BX|Player")
    bool IsInputLocked() const { return bInputLocked; }

    // --- 死亡 / 失神 ---

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void NotifyDeathOrUnconscious();

    // --- Save 連携 ---

    UFUNCTION(BlueprintCallable, Category="BX|Player|Save")
    void ExportPlayerSaveState(UPARAM(ref) FBXPlayerSaveState& OutState) const;

    UFUNCTION(BlueprintCallable, Category="BX|Player|Save")
    void ImportPlayerSaveState(const FBXPlayerSaveState& InState);

    // --- BlueprintImplementableEvent ---

    UFUNCTION(BlueprintImplementableEvent, Category="BX|Player")
    void BP_OnViewModeChanged(EBXViewMode NewViewMode);

    UFUNCTION(BlueprintImplementableEvent, Category="BX|Player")
    void BP_OnLocomotionStateChanged(EBXLocomotionState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category="BX|Player")
    void BP_OnCombatStateChanged(EBXCombatState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category="BX|Player")
    void BP_OnPlayerDeath();

    // =========================================================
    // 入力ハンドラ (SPEC §23-4)
    // =========================================================

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_MoveTriggered(const FInputActionValue& Value);

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_LookTriggered(const FInputActionValue& Value);

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_JumpTriggered();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_SprintStarted();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_SprintCompleted();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_CrouchToggled();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_ProneToggled();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_LeanLeftStarted();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_LeanLeftCompleted();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_LeanRightStarted();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_LeanRightCompleted();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_FirePrimaryStarted();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_FirePrimaryCompleted();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_FireSecondaryTriggered();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_ReloadTriggered();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_SwitchWeaponSlot(EBXWeaponSlot WeaponSlot);

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_HolsterTriggered();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_InteractTriggered();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_MedicalUseTriggered();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_QuickSlotTriggered(int32 QuickSlotIndex);

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_OpenInventoryTriggered();

    UFUNCTION(BlueprintCallable, Category="BX|Player")
    void Input_OpenMapTriggered();
};
