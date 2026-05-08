// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ABXOcclusionVolume.generated.h"

class UBoxComponent;

// AI 視線遮断ボリューム (Sprint 28)
// このボリューム内に入ったアクターは AI の PerceptionSystem で "occluded" 扱いになる。
// BXAlertSubsystem の BroadcastAlertToEnemies 時に距離フィルタ補正に使用。
// 壁・建物内部への配置を想定。レベル配置時に BoxExtent を調整すること。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXOcclusionVolume : public AActor
{
    GENERATED_BODY()

public:
    ABXOcclusionVolume();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BX|Map")
    TObjectPtr<UBoxComponent> VolumeBox;

    // このボリューム内からの Alert 伝播を遮断する (false なら通常伝播)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Map")
    bool bBlocksAlertPropagation = true;

    // ボリューム内からの AI Sight 判定を遮断する
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Map")
    bool bBlocksAISight = true;

    // デバッグ表示 (ゲーム中は非表示)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Debug")
    bool bShowDebugBox = false;

    // このボリューム内に指定座標が含まれるか
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Map")
    bool ContainsLocation(FVector Location) const;

    // 登録済み Occlusion ボリュームのグローバルリスト (BXAlertSubsystem から参照)
    static TArray<TWeakObjectPtr<ABXOcclusionVolume>>& GetRegisteredVolumes();

protected:
    virtual void DrawDebug(float DeltaTime);

private:
    static TArray<TWeakObjectPtr<ABXOcclusionVolume>> RegisteredVolumes;
};
