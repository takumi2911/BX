// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/BXEnums.h"

#include "FBXSpawnPointRow.generated.h"

// DT_BX_SpawnPoints 行構造体 - レイド開始スポーン地点 (Sprint 25)
// 実際の座標はレベル上の ABXSpawnPointActor に持たせる。
// この DataTable はメタデータ (分類 / 表示名 / デフォルト可否) のみ管理する。
USTRUCT(BlueprintType)
struct BX_API FBXSpawnPointRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|SpawnPoint")
    FName SpawnPointId;

    // 紐付くロケーション ID (DT_BX_Locations の行キーと一致させること)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|SpawnPoint")
    FName LocationId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|SpawnPoint")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|SpawnPoint")
    EBXSpawnType SpawnType = EBXSpawnType::Default;

    // ランダムスポーン選択時に最初の候補になるか
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|SpawnPoint")
    bool bIsDefault = false;

    // スポーン不可条件 (将来的に条件式文字列で拡張予定、現在は常時有効)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BX|SpawnPoint")
    bool bIsAvailable = true;
};
