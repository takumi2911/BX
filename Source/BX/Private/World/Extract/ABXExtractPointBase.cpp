// Copyright (c) 2026 BLACKOUT EXFIL. All rights reserved.

#include "World/Extract/ABXExtractPointBase.h"
#include "Data/Extract/FBXExtractRuleRow.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "TimerManager.h"

static const FName TAG_CANCEL_DAMAGE = TEXT("Extract.Cancel.Damage");
static const FName TAG_CANCEL_MOVEOUT = TEXT("Extract.Cancel.MoveOut");
static const FName TAG_CANCEL_AIM    = TEXT("Extract.Cancel.Aim");
static const FName TAG_CANCEL_ITEM   = TEXT("Extract.Cancel.ItemUse");

ABXExtractPointBase::ABXExtractPointBase()
{
    PrimaryActorTick.bCanEverTick = true;

    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    TriggerVolume->SetBoxExtent(FVector(150.0f));
    TriggerVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    RootComponent = TriggerVolume;

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(RootComponent);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ExtractEvaluator = CreateDefaultSubobject<UAC_BX_ExtractEvaluator>(TEXT("ExtractEvaluator"));
}

void ABXExtractPointBase::BeginPlay()
{
    Super::BeginPlay();

    TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABXExtractPointBase::OnTriggerBeginOverlap);
    TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ABXExtractPointBase::OnTriggerEndOverlap);
}

void ABXExtractPointBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsExtractionRunning) { return; }

    float Elapsed = GetWorld()->GetTimeSeconds() - ExtractionStartTimeSec;
    float Ratio   = FMath::Clamp(Elapsed / FMath::Max(ExtractionCountdownSec, KINDA_SMALL_NUMBER), 0.0f, 1.0f);
    OnProgressChanged.Broadcast(Ratio);
}

float ABXExtractPointBase::GetProgressRatio() const
{
    if (!bIsExtractionRunning || ExtractionCountdownSec <= 0.0f) { return 0.0f; }
    float Elapsed = GetWorld()->GetTimeSeconds() - ExtractionStartTimeSec;
    return FMath::Clamp(Elapsed / ExtractionCountdownSec, 0.0f, 1.0f);
}

void ABXExtractPointBase::StartExtraction()
{
    if (bIsExtractionRunning || bIsExtractionLocked || !bHasSatisfiedConditions) { return; }

    UDataTable* Table = ExtractEvaluator->ExtractRuleDataTable.Get();
    if (!Table) { return; }
    const FBXExtractRuleRow* Row = Table->FindRow<FBXExtractRuleRow>(ExtractId, TEXT(""));
    if (!Row) { return; }

    bIsExtractionRunning     = true;
    ExtractionStartTimeSec   = GetWorld()->GetTimeSeconds();
    ExtractionCountdownSec   = Row->CountdownTimeSec;

    BP_OnExtractionStarted();

    if (Row->CountdownTimeSec <= 0.0f)
    {
        OnExtractionComplete();
    }
    else
    {
        GetWorld()->GetTimerManager().SetTimer(
            ExtractionTimer,
            this,
            &ABXExtractPointBase::OnExtractionComplete,
            Row->CountdownTimeSec,
            false
        );

        // 増援発火
        if (Row->ReinforcementWaveId != NAME_None)
        {
            BroadcastEvent(false, FGameplayTag{}, Row->ReinforcementWaveId);
        }
    }
}

void ABXExtractPointBase::NotifyDamageReceived()
{
    if (!bIsExtractionRunning) { return; }
    UDataTable* Table = ExtractEvaluator->ExtractRuleDataTable.Get();
    const FBXExtractRuleRow* Row = Table ? Table->FindRow<FBXExtractRuleRow>(ExtractId, TEXT("")) : nullptr;
    if (Row && Row->bCancelOnDamage)
    {
        CancelExtraction(FGameplayTag::RequestGameplayTag(TAG_CANCEL_DAMAGE));
    }
}

void ABXExtractPointBase::NotifyAimStarted()
{
    if (!bIsExtractionRunning) { return; }
    UDataTable* Table = ExtractEvaluator->ExtractRuleDataTable.Get();
    const FBXExtractRuleRow* Row = Table ? Table->FindRow<FBXExtractRuleRow>(ExtractId, TEXT("")) : nullptr;
    if (Row && Row->bCancelOnAim)
    {
        CancelExtraction(FGameplayTag::RequestGameplayTag(TAG_CANCEL_AIM));
    }
}

void ABXExtractPointBase::NotifyItemUsed()
{
    if (!bIsExtractionRunning) { return; }
    UDataTable* Table = ExtractEvaluator->ExtractRuleDataTable.Get();
    const FBXExtractRuleRow* Row = Table ? Table->FindRow<FBXExtractRuleRow>(ExtractId, TEXT("")) : nullptr;
    if (Row && Row->bCancelOnUseItem)
    {
        CancelExtraction(FGameplayTag::RequestGameplayTag(TAG_CANCEL_ITEM));
    }
}

void ABXExtractPointBase::ReevaluateConditions()
{
    // BP 側から CarriedItemIds 等を渡せないため、デフォルト引数で再評価
    // 実際の使用では BP から EvaluateConditions を直接呼ぶことを推奨
    FBXExtractEvalResult NewResult = ExtractEvaluator->EvaluateConditions(
        ExtractId, {}, 0, 0.0f, {}, NAME_None);

    bool bChanged = (NewResult.bCanUseNow != LastEvalResult.bCanUseNow);
    LastEvalResult = NewResult;
    bHasSatisfiedConditions = NewResult.bCanUseNow;

    if (bChanged)
    {
        OnConditionsChanged.Broadcast(LastEvalResult);
    }
}

void ABXExtractPointBase::OnTriggerBeginOverlap(UPrimitiveComponent* /*OverlappedComp*/,
    AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/,
    int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
    if (!OtherActor || !OtherActor->ActorHasTag(TEXT("Player"))) { return; }
    bIsPlayerInsideTrigger = true;
    ReevaluateConditions();
}

void ABXExtractPointBase::OnTriggerEndOverlap(UPrimitiveComponent* /*OverlappedComp*/,
    AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/)
{
    if (!OtherActor || !OtherActor->ActorHasTag(TEXT("Player"))) { return; }
    bIsPlayerInsideTrigger = false;

    UDataTable* Table = ExtractEvaluator->ExtractRuleDataTable.Get();
    const FBXExtractRuleRow* Row = Table ? Table->FindRow<FBXExtractRuleRow>(ExtractId, TEXT("")) : nullptr;
    if (bIsExtractionRunning && Row && Row->bCancelOnMoveOut)
    {
        CancelExtraction(FGameplayTag::RequestGameplayTag(TAG_CANCEL_MOVEOUT));
    }
}

void ABXExtractPointBase::OnExtractionComplete()
{
    bIsExtractionRunning = false;

    UDataTable* Table = ExtractEvaluator->ExtractRuleDataTable.Get();
    const FBXExtractRuleRow* Row = Table ? Table->FindRow<FBXExtractRuleRow>(ExtractId, TEXT("")) : nullptr;
    if (Row && Row->bOneWayOnly)
    {
        bIsExtractionLocked = true;
    }

    BP_OnExtractionCompleted();
    BroadcastEvent(true, FGameplayTag{}, NAME_None);
}

void ABXExtractPointBase::CancelExtraction(FGameplayTag Reason)
{
    GetWorld()->GetTimerManager().ClearTimer(ExtractionTimer);
    bIsExtractionRunning = false;
    ExtractionStartTimeSec = 0.0f;

    BP_OnExtractionCancelled(Reason);
    BroadcastEvent(false, Reason, NAME_None);
}

void ABXExtractPointBase::BroadcastEvent(bool bSuccess, FGameplayTag CancelReason, FName WaveId)
{
    FBXExtractEventData Data;
    Data.ExtractId       = ExtractId;
    Data.bWasSuccessful  = bSuccess;
    Data.StartTimeSec    = ExtractionStartTimeSec;
    Data.EndTimeSec      = GetWorld()->GetTimeSeconds();
    Data.CancelReasonTag = CancelReason;
    Data.TriggeredWaveId = WaveId;

    UDataTable* Table = ExtractEvaluator->ExtractRuleDataTable.Get();
    const FBXExtractRuleRow* Row = Table ? Table->FindRow<FBXExtractRuleRow>(ExtractId, TEXT("")) : nullptr;
    if (Row) { Data.RegionId = Row->RegionId; }

    OnExtractEvent.Broadcast(Data);
}
