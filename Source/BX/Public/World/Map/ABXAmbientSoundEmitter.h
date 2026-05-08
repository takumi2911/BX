// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ABXAmbientSoundEmitter.generated.h"

class UAudioComponent;
class USoundCue;
class USphereComponent;

UENUM(BlueprintType)
enum class EBXAmbientSoundType : uint8
{
    Continuous,   // 常時再生
    RandomLoop,   // ランダム間隔でループ
    OneShot,      // BeginPlay で 1 回のみ
};

// 環境音発生器 (Sprint 28)
// 距離ベースの空間音響。Tarkov 風の工場音・外気音・機械音などを配置。
// UE5 のデフォルト Audio Distance Attenuation を利用する。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXAmbientSoundEmitter : public AActor
{
    GENERATED_BODY()

public:
    ABXAmbientSoundEmitter();

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Components")
    TObjectPtr<UAudioComponent> AudioComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Debug")
    TObjectPtr<USphereComponent> DebugSphere;

    // 再生するサウンド
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Audio")
    TObjectPtr<USoundCue> AmbientSound;

    // 再生タイプ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Audio")
    EBXAmbientSoundType SoundType = EBXAmbientSoundType::Continuous;

    // RandomLoop の最小・最大インターバル (秒)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Audio",
        meta=(EditCondition="SoundType==EBXAmbientSoundType::RandomLoop"))
    float MinInterval = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Audio",
        meta=(EditCondition="SoundType==EBXAmbientSoundType::RandomLoop"))
    float MaxInterval = 15.0f;

    // 音量スケール
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Audio",
        meta=(ClampMin="0.0", ClampMax="2.0"))
    float VolumeMultiplier = 1.0f;

    UFUNCTION(BlueprintCallable, Category="BX|Audio")
    void PlaySound();

    UFUNCTION(BlueprintCallable, Category="BX|Audio")
    void StopSound();

private:
    FTimerHandle RandomLoopTimer;
    void ScheduleNextPlay();
};
