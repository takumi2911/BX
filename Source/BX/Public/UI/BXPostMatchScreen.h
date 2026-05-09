#pragma once
// BXPostMatchScreen - 試合終了結果画面ウィジェット (Sprint 29)

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Systems/BXMatchSubsystem.h"
#include "BXPostMatchScreen.generated.h"

class UTextBlock;
class UButton;
class UImage;

UCLASS(BlueprintType, Blueprintable)
class BX_API UBXPostMatchScreen : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // BXMatchSubsystem::OnMatchEnded に接続して呼ぶ
    UFUNCTION(BlueprintCallable, Category="BX|UI|PostMatch")
    void SetResult(const FBXRaidResult& Result);

    // Blueprint で実装: アニメーション演出など
    UFUNCTION(BlueprintImplementableEvent, Category="BX|UI|PostMatch")
    void BP_OnResultSet(bool bExtracted);

protected:
    // --- ウィジェット参照 (Blueprint で BindWidget) ---

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> ResultTitleText;    // "EXTRACTION SUCCESS" / "KIA"

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> SurvivalTimeText;   // "生存時間: 12:34"

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> EnemiesKilledText;  // "撃破数: 3"

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> LootValueText;      // "持ち帰り: ¥250,000"

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UButton> ReturnToLobbyButton;

    UFUNCTION()
    void OnReturnToLobbyClicked();

private:
    FBXRaidResult CachedResult;
    static FString FormatTime(float Seconds);
};
