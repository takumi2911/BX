#pragma once
// BXMatchGameMode - レイドマップ用 GameMode (Sprint 29)

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BXMatchGameMode.generated.h"

class UBXMatchSubsystem;

UCLASS(BlueprintType, Blueprintable)
class BX_API ABXMatchGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ABXMatchGameMode();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // レイド制限時間 (秒) - 0 = 無制限
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BX|Match")
    float RaidTimeLimitSec = 1800.f;  // 30 分

    // PreGame カウントダウン秒数
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BX|Match")
    int32 PreGameCountdownSec = 3;

    // プレイヤーが死亡した時に呼ぶ
    UFUNCTION(BlueprintCallable, Category="BX|Match")
    void OnPlayerDied();

    UFUNCTION(BlueprintCallable, Category="BX|Match")
    void OnPlayerReachedExtract(FName ExtractPointID);

protected:
    UBXMatchSubsystem* GetMatchSubsystem() const;

    FTimerHandle RaidTimerHandle;
    void OnRaidTimeUp();
};
