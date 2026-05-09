#pragma once
// BXLobbyWidget - ロビーメイン画面 (Sprint 29)

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BXLobbyWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS(BlueprintType, Blueprintable)
class BX_API UBXLobbyWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> TitleText;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UButton>  StartRaidButton;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UButton>  SettingsButton;

    UFUNCTION() void OnStartRaidClicked();
    UFUNCTION() void OnSettingsClicked();

    // 出撃するマップ名 (Blueprint で変更可)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Lobby")
    FName RaidMapName = TEXT("LV_KansaiIndustrial_01");
};
