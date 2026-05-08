// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ABXMapBoundary.generated.h"

class UBoxComponent;
class APlayerCharacterBase;

UENUM(BlueprintType)
enum class EBXBoundaryAction : uint8
{
    DamageOverTime,   // 境界外にいる間ダメージ
    TeleportBack,     // 即座に境界内に戻す
    Kill,             // 即死
};

// マップ境界アクター (Sprint 28)
// プレイヤーが境界外に出た場合の処理を管理。
// ABXExtractPointBase の抽出ゾーンとは独立して機能する。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXMapBoundary : public AActor
{
    GENERATED_BODY()

public:
    ABXMapBoundary();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UBoxComponent> BoundaryVolume;

    // 境界外になったときの処理
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Boundary")
    EBXBoundaryAction OutsideAction = EBXBoundaryAction::DamageOverTime;

    // DamageOverTime の場合: 1 秒あたりのダメージ量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Boundary",
        meta=(EditCondition="OutsideAction==EBXBoundaryAction::DamageOverTime", ClampMin="0"))
    float DamagePerSecond = 25.0f;

    // 境界外に出てから処理が始まるまでの猶予時間 (秒)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Boundary")
    float GracePeriodSec = 5.0f;

    // HUD に表示する警告テキスト
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Boundary")
    FText WarningText = FText::FromString(TEXT("マップ外です。戻ってください。"));

    // --- BlueprintImplementableEvent ---
    UFUNCTION(BlueprintImplementableEvent, Category="BX|Boundary")
    void BP_OnPlayerExitedBoundary(APlayerCharacterBase* Player);

    UFUNCTION(BlueprintImplementableEvent, Category="BX|Boundary")
    void BP_OnPlayerEnteredBoundary(APlayerCharacterBase* Player);

protected:
    UFUNCTION()
    void OnVolumeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
    TMap<APlayerCharacterBase*, float> OutsideTimestamps;
    void ApplyBoundaryAction(APlayerCharacterBase* Player, float DeltaTime);
};
