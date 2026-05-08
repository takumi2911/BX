// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/BXEnums.h"

#include "BXEnemyCharacter.generated.h"

class UAC_BX_HealthBodyParts;
class UAC_BX_ArmorEquipment;
class UAC_BX_StatusEffects;
class UAC_BX_WeaponHandler;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDied, ABXEnemyCharacter*, Enemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyStateChanged, ABXEnemyCharacter*, Enemy, EBXEnemyState, NewState);

// AI 敵キャラクター基底クラス (Sprint 24)
// プレイヤーと共通コンポーネント群を持ち、BT で制御される。
// BT グラフ結線は手動 (BT_SETUP_GUIDE.md 参照)
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABXEnemyCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // --- コンポーネント ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Enemy|Components")
    TObjectPtr<UAC_BX_HealthBodyParts> HealthBodyParts;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Enemy|Components")
    TObjectPtr<UAC_BX_ArmorEquipment> ArmorEquipment;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Enemy|Components")
    TObjectPtr<UAC_BX_StatusEffects> StatusEffects;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Enemy|Components")
    TObjectPtr<UAC_BX_WeaponHandler> WeaponHandler;

    // --- アーキタイプ ---

    // DT_BX_EnemyArchetypes の行キー。BeginPlay で InitFromArchetype() が呼ばれる
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Enemy")
    FName ArchetypeId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Enemy")
    TObjectPtr<UDataTable> EnemyArchetypeDataTable;

    // --- 状態 ---

    UPROPERTY(BlueprintReadOnly, Category="BX|Enemy")
    EBXEnemyState EnemyState = EBXEnemyState::Idle;

    UFUNCTION(BlueprintCallable, Category="BX|Enemy")
    void SetEnemyState(EBXEnemyState NewState);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Enemy")
    bool IsDead() const { return EnemyState == EBXEnemyState::Dead; }

    // --- ダメージ受付 ---

    // BT / ダメージ計算システムから直接呼ぶ
    UFUNCTION(BlueprintCallable, Category="BX|Enemy")
    void TakeBodyPartDamage(EBXBodyPart Part, float Damage);

    // UE5 標準 TakeDamage は Chest ヒット扱いでフォールスルー
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    // --- 攻撃 ---

    // BT Task から呼ぶ: TargetActor に向けて WeaponHandler 発砲
    UFUNCTION(BlueprintCallable, Category="BX|Enemy")
    void FireAtTarget(AActor* Target);

    // --- パトロール ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Enemy|Patrol")
    TArray<TObjectPtr<AActor>> PatrolPoints;

    UFUNCTION(BlueprintCallable, Category="BX|Enemy|Patrol")
    AActor* GetNextPatrolPoint();

    // --- Delegates ---

    UPROPERTY(BlueprintAssignable, Category="BX|Enemy")
    FOnEnemyDied OnEnemyDied;

    UPROPERTY(BlueprintAssignable, Category="BX|Enemy")
    FOnEnemyStateChanged OnEnemyStateChanged;

protected:
    int32 PatrolIndex = 0;
    float LastFireTimeSec = 0.0f;
    float AttackCooldownSec = 0.8f;
    float BaseAccuracy = 0.55f;

    void InitFromArchetype();
    void Die();
    void ApplyRagdoll();
};
