// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/BXEnums.h"

#include "BXSpawnPointActor.generated.h"

// レベル上に配置するスポーンポイントマーカー (Sprint 25)
// BXLocationManager::GetDefaultSpawnPoint() で取得した SpawnPointId と
// SpawnPointId プロパティが一致するアクターを検索してスポーン座標を得る。
UCLASS(BlueprintType, Blueprintable)
class BX_API ABXSpawnPointActor : public AActor
{
    GENERATED_BODY()

public:
    ABXSpawnPointActor();

    // DT_BX_SpawnPoints の行キーと一致させること
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|SpawnPoint")
    FName SpawnPointId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|SpawnPoint")
    EBXSpawnType SpawnType = EBXSpawnType::Default;

    // 便利関数: SpawnPointId でレベルから検索する
    UFUNCTION(BlueprintCallable, Category="BX|SpawnPoint", meta=(WorldContext="WorldContextObject"))
    static ABXSpawnPointActor* FindBySpawnPointId(UObject* WorldContextObject, FName InSpawnPointId);
};
