// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "FBXGlobalSettings.generated.h"

// グローバル設定（音量・感度）(CLAUDE.md §6-2)
// キーコンフィグ詳細構造は §16-3 追記後に拡張 → docs/QUESTIONS.md Q-01 参照
USTRUCT(BlueprintType)
struct BX_API FBXGlobalSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Settings")
    int32 SaveVersion = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Settings", meta=(ClampMin="0.0", ClampMax="1.0"))
    float MasterVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Settings", meta=(ClampMin="0.0", ClampMax="1.0"))
    float BGMVolume = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Settings", meta=(ClampMin="0.0", ClampMax="1.0"))
    float SEVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Settings", meta=(ClampMin="0.0", ClampMax="1.0"))
    float VoiceVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Settings", meta=(ClampMin="0.01", ClampMax="10.0"))
    float MouseSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BX|Save|Settings", meta=(ClampMin="0.01", ClampMax="10.0"))
    float ControllerSensitivity = 1.0f;
};
