// Copyright (c) takumi2911 - BlackoutExfil
#include "UI/BXLobbyWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/BXLobbyGameMode.h"

void UBXLobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (TitleText)
    {
        TitleText->SetText(FText::FromString(TEXT("BLACKOUT EXFIL")));
    }
    if (StartRaidButton)
    {
        StartRaidButton->OnClicked.AddDynamic(this, &UBXLobbyWidget::OnStartRaidClicked);
    }
    if (SettingsButton)
    {
        SettingsButton->OnClicked.AddDynamic(this, &UBXLobbyWidget::OnSettingsClicked);
    }
}

void UBXLobbyWidget::OnStartRaidClicked()
{
    if (ABXLobbyGameMode* GM = Cast<ABXLobbyGameMode>(
            UGameplayStatics::GetGameMode(this)))
    {
        GM->StartRaid(RaidMapName);
    }
    else
    {
        UGameplayStatics::OpenLevel(this, RaidMapName);
    }
}

void UBXLobbyWidget::OnSettingsClicked()
{
    // TODO: 設定画面を開く
    UE_LOG(LogTemp, Log, TEXT("[Lobby] Settings clicked (not yet implemented)"));
}
