// Copyright (c) takumi2911 - BlackoutExfil
#include "UI/BXPostMatchScreen.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/BXMatchSubsystem.h"

void UBXPostMatchScreen::NativeConstruct()
{
    Super::NativeConstruct();
    if (ReturnToLobbyButton)
    {
        ReturnToLobbyButton->OnClicked.AddDynamic(this, &UBXPostMatchScreen::OnReturnToLobbyClicked);
    }
}

void UBXPostMatchScreen::SetResult(const FBXRaidResult& Result)
{
    CachedResult = Result;

    if (ResultTitleText)
    {
        ResultTitleText->SetText(Result.ResultMessage);
    }
    if (SurvivalTimeText)
    {
        SurvivalTimeText->SetText(
            FText::FromString(FString::Printf(TEXT("生存時間: %s"), *FormatTime(Result.SurvivalTime))));
    }
    if (EnemiesKilledText)
    {
        EnemiesKilledText->SetText(
            FText::FromString(FString::Printf(TEXT("撃破数: %d"), Result.EnemiesKilled)));
    }
    if (LootValueText)
    {
        LootValueText->SetText(
            FText::FromString(FString::Printf(TEXT("持ち帰り: %.0f"), Result.LootValue)));
    }

    BP_OnResultSet(Result.bExtracted);
}

void UBXPostMatchScreen::OnReturnToLobbyClicked()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UBXMatchSubsystem* MS = GI->GetSubsystem<UBXMatchSubsystem>())
        {
            MS->ReturnToLobby();
        }
    }
}

FString UBXPostMatchScreen::FormatTime(float Seconds)
{
    const int32 Mins = FMath::FloorToInt(Seconds / 60.f);
    const int32 Secs = FMath::FloorToInt(Seconds) % 60;
    return FString::Printf(TEXT("%02d:%02d"), Mins, Secs);
}
