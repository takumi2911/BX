// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ABXPlayerStartGroup.generated.h"

class APlayerStart;

UENUM(BlueprintType)
enum class EBXSpawnStrategy : uint8
{
    Random,          // ランダム
    Sequential,      // 順番
    LeastRecent,     // 最後に使われてから最も時間が経ったもの
};

// 複数スポーンポイント管理グループ (Sprint 28)
// ABXGameMode::ChoosePlayerStart() から参照される。
// DT_BX_SpawnPoints (Sprint 25) の SpawnGroupId でグループを特定。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXPlayerStartGroup : public AActor
{
    GENERATED_BODY()

public:
    ABXPlayerStartGroup();

    // このグループを識別するID (DT_BX_SpawnPoints の SpawnGroupId と一致させる)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Spawn")
    FName GroupId;

    // 管理下の PlayerStart アクター
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Spawn")
    TArray<TObjectPtr<APlayerStart>> SpawnPoints;

    // スポーン戦略
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Spawn")
    EBXSpawnStrategy Strategy = EBXSpawnStrategy::Random;

    // 次のスポーン地点を返す (ABXGameMode から呼ぶ)
    UFUNCTION(BlueprintCallable, Category="BX|Spawn")
    APlayerStart* PickNextSpawnPoint();

    // スポーン地点が使用されたことを記録
    UFUNCTION(BlueprintCallable, Category="BX|Spawn")
    void RecordSpawnUsed(APlayerStart* UsedPoint);

    // このグループが有効なスポーン地点を持つか
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BX|Spawn")
    bool HasValidSpawnPoints() const;

private:
    int32 SequentialIndex = 0;
    TMap<APlayerStart*, float> LastUsedTimestamps;
};
