#pragma once
// BXLobbyGameMode - ロビーマップ用 GameMode (Sprint 29)

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Sound/SoundBase.h"
#include "BXLobbyGameMode.generated.h"

UCLASS(BlueprintType, Blueprintable)
class BX_API ABXLobbyGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ABXLobbyGameMode();
    virtual void BeginPlay() override;

    // プレイヤーがレイド開始ボタンを押したら呼ぶ
    UFUNCTION(BlueprintCallable, Category="BX|Lobby")
    void StartRaid(FName MapName = TEXT("LV_KansaiIndustrial_01"));

protected:
    // ロービー BGM (オプション)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BX|Lobby")
    TObjectPtr<USoundBase> LobbyBGM;
};
