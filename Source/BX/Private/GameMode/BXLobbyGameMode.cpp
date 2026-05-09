// Copyright (c) takumi2911 - BlackoutExfil
#include "GameMode/BXLobbyGameMode.h"
#include "Kismet/GameplayStatics.h"

ABXLobbyGameMode::ABXLobbyGameMode()
{
    // ロビー専用 HUD / Pawn は Blueprint 子クラスで設定
}

void ABXLobbyGameMode::BeginPlay()
{
    Super::BeginPlay();
    if (LobbyBGM)
    {
        UGameplayStatics::PlaySound2D(this, LobbyBGM);
    }
}

void ABXLobbyGameMode::StartRaid(FName MapName)
{
    UGameplayStatics::OpenLevel(this, MapName);
}
