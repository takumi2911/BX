// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "Save/USaveGame_BX.h"
#include "JsonObjectConverter.h"

const FString USaveGame_BX::SlotNamePrefix = TEXT("BX_Save_");

USaveGame_BX::USaveGame_BX()
{
}

FString USaveGame_BX::DebugDumpJson() const
{
    FString JsonOut;
    FJsonObjectConverter::UStructToJsonObjectString(StaticClass(), this, JsonOut, 0, 0);
    return JsonOut;
}
