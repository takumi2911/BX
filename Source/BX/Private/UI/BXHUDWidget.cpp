// Copyright (c) takumi2911 - BlackoutExfil
#include "UI/BXHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "Characters/APlayerCharacterBase.h"

TSharedRef<SWidget> UBXHUDWidget::RebuildWidget()
{
    if (!RootCanvas)
    {
        RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
        WidgetTree->RootWidget = RootCanvas;
        BuildLayout();
    }
    return Super::RebuildWidget();
}

void UBXHUDWidget::BuildLayout()
{
    if (!RootCanvas) { return; }

    // クロスヘア (中央)
    Crosshair = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Crosshair"));
    Crosshair->SetBrushColor(FLinearColor::White);
    Crosshair->SetPadding(FMargin(0.f));
    if (UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(Crosshair))
    {
        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        CanvasSlot->SetPosition(FVector2D(0.f, 0.f));
        CanvasSlot->SetSize(FVector2D(4.f, 4.f));
        CanvasSlot->SetAutoSize(false);
        CanvasSlot->SetZOrder(10);
    }

    // 弾薬テキスト (右下)
    AmmoText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("AmmoText"));
    AmmoText->SetText(FText::FromString(TEXT("-- / --")));
    {
        FSlateFontInfo Font = AmmoText->GetFont();
        Font.Size = 32;
        AmmoText->SetFont(Font);
    }
    AmmoText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    if (UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(AmmoText))
    {
        CanvasSlot->SetAnchors(FAnchors(1.f, 1.f, 1.f, 1.f));
        CanvasSlot->SetAlignment(FVector2D(1.f, 1.f));
        CanvasSlot->SetPosition(FVector2D(-40.f, -40.f));
        CanvasSlot->SetAutoSize(true);
        CanvasSlot->SetZOrder(5);
    }

    // HP バー (左下・7 部位)
    HPVerticalBox = WidgetTree->ConstructWidget<UVerticalBox>(
        UVerticalBox::StaticClass(), TEXT("HPVerticalBox"));
    if (UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(HPVerticalBox))
    {
        CanvasSlot->SetAnchors(FAnchors(0.f, 1.f, 0.f, 1.f));
        CanvasSlot->SetAlignment(FVector2D(0.f, 1.f));
        CanvasSlot->SetPosition(FVector2D(20.f, -20.f));
        CanvasSlot->SetSize(FVector2D(250.f, 280.f));
        CanvasSlot->SetAutoSize(false);
        CanvasSlot->SetZOrder(5);
    }

    PB_HP_Head     = CreateBodyPartBar(HPVerticalBox, TEXT("PB_HP_Head"));
    PB_HP_Chest    = CreateBodyPartBar(HPVerticalBox, TEXT("PB_HP_Chest"));
    PB_HP_Abdomen  = CreateBodyPartBar(HPVerticalBox, TEXT("PB_HP_Abdomen"));
    PB_HP_LeftArm  = CreateBodyPartBar(HPVerticalBox, TEXT("PB_HP_LeftArm"));
    PB_HP_RightArm = CreateBodyPartBar(HPVerticalBox, TEXT("PB_HP_RightArm"));
    PB_HP_LeftLeg  = CreateBodyPartBar(HPVerticalBox, TEXT("PB_HP_LeftLeg"));
    PB_HP_RightLeg = CreateBodyPartBar(HPVerticalBox, TEXT("PB_HP_RightLeg"));

    // Sprint 19: 防具耐久度バー (右下)
    ArmorVerticalBox = WidgetTree->ConstructWidget<UVerticalBox>(
        UVerticalBox::StaticClass(), TEXT("ArmorVerticalBox"));
    if (UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(ArmorVerticalBox))
    {
        CanvasSlot->SetAnchors(FAnchors(1.f, 1.f, 1.f, 1.f));
        CanvasSlot->SetAlignment(FVector2D(1.f, 1.f));
        CanvasSlot->SetPosition(FVector2D(-40.f, -90.f));
        CanvasSlot->SetSize(FVector2D(160.f, 90.f));
        CanvasSlot->SetAutoSize(false);
        CanvasSlot->SetZOrder(5);
    }

    PB_Armor_Head    = CreateArmorBar(ArmorVerticalBox, TEXT("PB_Armor_Head"));
    PB_Armor_Chest   = CreateArmorBar(ArmorVerticalBox, TEXT("PB_Armor_Chest"));
    PB_Armor_Abdomen = CreateArmorBar(ArmorVerticalBox, TEXT("PB_Armor_Abdomen"));

    // Sprint 20: 状態異常アイコン (下中央)
    StatusIconBox = WidgetTree->ConstructWidget<UHorizontalBox>(
        UHorizontalBox::StaticClass(), TEXT("StatusIconBox"));
    if (UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(StatusIconBox))
    {
        CanvasSlot->SetAnchors(FAnchors(0.5f, 1.f, 0.5f, 1.f));
        CanvasSlot->SetAlignment(FVector2D(0.5f, 1.f));
        CanvasSlot->SetPosition(FVector2D(0.f, -20.f));
        CanvasSlot->SetSize(FVector2D(192.f, 28.f));
        CanvasSlot->SetAutoSize(false);
        CanvasSlot->SetZOrder(5);
    }

    StatusIcon_Bleed       = CreateStatusIcon(StatusIconBox, TEXT("SI_Bleed"));
    StatusIcon_Fracture    = CreateStatusIcon(StatusIconBox, TEXT("SI_Fracture"));
    StatusIcon_Pain        = CreateStatusIcon(StatusIconBox, TEXT("SI_Pain"));
    StatusIcon_Dehydration = CreateStatusIcon(StatusIconBox, TEXT("SI_Dehydration"));
    StatusIcon_Fatigue     = CreateStatusIcon(StatusIconBox, TEXT("SI_Fatigue"));
    StatusIcon_Neuro       = CreateStatusIcon(StatusIconBox, TEXT("SI_Neuro"));
}

UProgressBar* UBXHUDWidget::CreateBodyPartBar(UVerticalBox* Parent, FName Name)
{
    if (!Parent) { return nullptr; }
    UProgressBar* Bar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), Name);
    Bar->SetPercent(1.0f);
    Bar->SetFillColorAndOpacity(ColorForRatio(1.0f));
    if (UVerticalBoxSlot* BoxSlot = Parent->AddChildToVerticalBox(Bar))
    {
        BoxSlot->SetPadding(FMargin(0.f, 2.f, 0.f, 2.f));
        FSlateChildSize ChildSize;
        ChildSize.SizeRule = ESlateSizeRule::Fill;
        ChildSize.Value = 1.f;
        BoxSlot->SetSize(ChildSize);
        BoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        BoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
    }
    return Bar;
}

UProgressBar* UBXHUDWidget::CreateArmorBar(UVerticalBox* Parent, FName Name)
{
    if (!Parent) { return nullptr; }
    UProgressBar* Bar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), Name);
    Bar->SetPercent(0.0f);
    Bar->SetFillColorAndOpacity(ArmorColorForRatio(0.0f));
    if (UVerticalBoxSlot* BoxSlot = Parent->AddChildToVerticalBox(Bar))
    {
        BoxSlot->SetPadding(FMargin(0.f, 1.f, 0.f, 1.f));
        FSlateChildSize ChildSize;
        ChildSize.SizeRule = ESlateSizeRule::Fill;
        ChildSize.Value = 1.f;
        BoxSlot->SetSize(ChildSize);
        BoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        BoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
    }
    return Bar;
}

UBorder* UBXHUDWidget::CreateStatusIcon(UHorizontalBox* Parent, FName Name)
{
    if (!Parent) { return nullptr; }
    UBorder* Icon = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name);
    Icon->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.2f, 0.3f));  // 初期: 非アクティブ
    Icon->SetPadding(FMargin(0.f));
    if (UHorizontalBoxSlot* BoxSlot = Parent->AddChildToHorizontalBox(Icon))
    {
        BoxSlot->SetPadding(FMargin(2.f, 0.f, 2.f, 0.f));
        FSlateChildSize ChildSize;
        ChildSize.SizeRule = ESlateSizeRule::Fill;
        ChildSize.Value = 1.f;
        BoxSlot->SetSize(ChildSize);
        BoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
        BoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
    }
    return Icon;
}

void UBXHUDWidget::InitializeForPlayer(APlayerCharacterBase* InPlayer)
{
    OwningPlayer = InPlayer;
}

void UBXHUDWidget::OnUpdateBodyPartHP(EBXBodyPart BodyPart, float HPRatio)
{
    if (UProgressBar* Bar = GetBarForBodyPart(BodyPart))
    {
        const float Clamped = FMath::Clamp(HPRatio, 0.f, 1.f);
        Bar->SetPercent(Clamped);
        Bar->SetFillColorAndOpacity(ColorForRatio(Clamped));
    }
}

void UBXHUDWidget::OnUpdateAmmo(int32 CurrentAmmo, int32 MagSize)
{
    if (AmmoText)
    {
        AmmoText->SetText(
            FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MagSize)));
    }
}

void UBXHUDWidget::OnUpdateCurrentWeapon(FName /*WeaponRowName*/) { }

void UBXHUDWidget::OnUpdateArmorDurability(EBXBodyPart BodyPart, float DurabilityRatio)
{
    if (UProgressBar* Bar = GetArmorBarForBodyPart(BodyPart))
    {
        const float Clamped = FMath::Clamp(DurabilityRatio, 0.f, 1.f);
        Bar->SetPercent(Clamped);
        Bar->SetFillColorAndOpacity(ArmorColorForRatio(Clamped));
    }
}

void UBXHUDWidget::OnUpdateStatusEffects(const TArray<EBXStatusType>& ActiveTypes)
{
    const FLinearColor Inactive(0.2f, 0.2f, 0.2f, 0.3f);

    auto SetIcon = [&](UBorder* Icon, EBXStatusType Type)
    {
        if (!Icon) { return; }
        const bool bActive = ActiveTypes.Contains(Type);
        Icon->SetBrushColor(bActive ? StatusColorForType(Type) : Inactive);
    };

    SetIcon(StatusIcon_Bleed,       EBXStatusType::Bleed);
    SetIcon(StatusIcon_Fracture,    EBXStatusType::Fracture);
    SetIcon(StatusIcon_Pain,        EBXStatusType::Pain);
    SetIcon(StatusIcon_Dehydration, EBXStatusType::Dehydration);
    SetIcon(StatusIcon_Fatigue,     EBXStatusType::Fatigue);
    SetIcon(StatusIcon_Neuro,       EBXStatusType::Neuro);
}

UProgressBar* UBXHUDWidget::GetBarForBodyPart(EBXBodyPart BodyPart) const
{
    switch (BodyPart)
    {
    case EBXBodyPart::Head:     return PB_HP_Head;
    case EBXBodyPart::Chest:    return PB_HP_Chest;
    case EBXBodyPart::Abdomen:  return PB_HP_Abdomen;
    case EBXBodyPart::LeftArm:  return PB_HP_LeftArm;
    case EBXBodyPart::RightArm: return PB_HP_RightArm;
    case EBXBodyPart::LeftLeg:  return PB_HP_LeftLeg;
    case EBXBodyPart::RightLeg: return PB_HP_RightLeg;
    default:                    return nullptr;
    }
}

UProgressBar* UBXHUDWidget::GetArmorBarForBodyPart(EBXBodyPart BodyPart) const
{
    switch (BodyPart)
    {
    case EBXBodyPart::Head:    return PB_Armor_Head;
    case EBXBodyPart::Chest:   return PB_Armor_Chest;
    case EBXBodyPart::Abdomen: return PB_Armor_Abdomen;
    default:                   return nullptr;
    }
}

UBorder* UBXHUDWidget::GetStatusIconForType(EBXStatusType Type) const
{
    switch (Type)
    {
    case EBXStatusType::Bleed:       return StatusIcon_Bleed;
    case EBXStatusType::Fracture:    return StatusIcon_Fracture;
    case EBXStatusType::Pain:        return StatusIcon_Pain;
    case EBXStatusType::Dehydration: return StatusIcon_Dehydration;
    case EBXStatusType::Fatigue:     return StatusIcon_Fatigue;
    case EBXStatusType::Neuro:       return StatusIcon_Neuro;
    default:                         return nullptr;
    }
}

FLinearColor UBXHUDWidget::ColorForRatio(float Ratio)
{
    if (Ratio > 0.66f) return FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
    if (Ratio > 0.33f) return FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);
    return FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

FLinearColor UBXHUDWidget::ArmorColorForRatio(float Ratio)
{
    if (Ratio > 0.66f) return FLinearColor(0.2f, 0.8f, 1.0f, 1.0f);
    if (Ratio > 0.33f) return FLinearColor(0.2f, 0.5f, 0.8f, 1.0f);
    if (Ratio > 0.0f)  return FLinearColor(0.5f, 0.2f, 0.5f, 1.0f);
    return FLinearColor(0.2f, 0.2f, 0.2f, 0.5f);
}

FLinearColor UBXHUDWidget::StatusColorForType(EBXStatusType Type)
{
    switch (Type)
    {
    case EBXStatusType::Bleed:       return FLinearColor(0.9f, 0.1f, 0.1f, 1.0f);  // 赤
    case EBXStatusType::Fracture:    return FLinearColor(0.2f, 0.5f, 1.0f, 1.0f);  // 青
    case EBXStatusType::Pain:        return FLinearColor(1.0f, 0.6f, 0.1f, 1.0f);  // オレンジ
    case EBXStatusType::Dehydration: return FLinearColor(0.7f, 0.5f, 0.2f, 1.0f);  // 茶
    case EBXStatusType::Fatigue:     return FLinearColor(0.5f, 0.5f, 0.3f, 1.0f);  // 黄緑
    case EBXStatusType::Neuro:       return FLinearColor(0.8f, 0.1f, 0.9f, 1.0f);  // 紫
    default:                         return FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
    }
}
