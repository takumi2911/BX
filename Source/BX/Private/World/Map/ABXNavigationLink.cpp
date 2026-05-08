// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.
#include "World/Map/ABXNavigationLink.h"

ABXNavigationLink::ABXNavigationLink()
{
    // NavLinkProxy はデフォルトで SmartLink を持つ
    // SmartLink を使用することで OnSmartLinkReached イベントが発火する
    SetSmartLinkEnabled(true);
}

void ABXNavigationLink::BeginPlay()
{
    Super::BeginPlay();
    OnSmartLinkReached.AddDynamic(this, &ABXNavigationLink::HandleSmartLinkReached);
}

void ABXNavigationLink::SetLinkEnabled(bool bEnabled)
{
    bIsLinkEnabled = bEnabled;
    SetSmartLinkEnabled(bEnabled);
}

void ABXNavigationLink::HandleSmartLinkReached(AActor* MovingActor, const FVector& Destination)
{
    if (!MovingActor || !bIsLinkEnabled) { return; }
    BP_OnAgentUsedLink(MovingActor);
}
