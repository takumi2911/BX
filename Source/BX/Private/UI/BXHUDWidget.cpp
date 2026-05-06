// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "UI/BXHUDWidget.h"
#include "Characters/APlayerCharacterBase.h"

void UBXHUDWidget::InitializeForPlayer(APlayerCharacterBase* Player)
{
    OwningPlayer = Player;
    UE_LOG(LogTemp, Log, TEXT("BXHUDWidget: SetOwningPlayer called — %s"),
        IsValid(Player) ? *Player->GetName() : TEXT("NULL"));
}
