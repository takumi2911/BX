// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/BXEnums.h"
#include "BXHUDWidget.generated.h"

class APlayerCharacterBase;
class UCanvasPanel;
class UVerticalBox;
class UHorizontalBox;
class UProgressBar;
class UTextBlock;
class UBorder;

UCLASS()
class BX_API UBXHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual TSharedRef<SWidget> RebuildWidget() override;

    UFUNCTION(BlueprintCallable, Category = "BX|HUD")
    void InitializeForPlayer(APlayerCharacterBase* InPlayer);

    UFUNCTION(BlueprintCallable, Category = "BX|HUD")
    void OnUpdateBodyPartHP(EBXBodyPart BodyPart, float HPRatio);

    UFUNCTION(BlueprintCallable, Category = "BX|HUD")
    void OnUpdateAmmo(int32 CurrentAmmo, int32 MagSize);

    UFUNCTION(BlueprintCallable, Category = "BX|HUD")
    void OnUpdateCurrentWeapon(FName WeaponRowName);

    // Sprint 19: 防具耐久度更新
    UFUNCTION(BlueprintCallable, Category = "BX|HUD")
    void OnUpdateArmorDurability(EBXBodyPart BodyPart, float DurabilityRatio);

    // Sprint 20: 状態異常アイコン更新
    UFUNCTION(BlueprintCallable, Category = "BX|HUD")
    void OnUpdateStatusEffects(const TArray<EBXStatusType>& ActiveTypes);

protected:
    UPROPERTY(Transient) TObjectPtr<UCanvasPanel>    RootCanvas;
    UPROPERTY(Transient) TObjectPtr<UBorder>         Crosshair;
    UPROPERTY(Transient) TObjectPtr<UTextBlock>      AmmoText;
    UPROPERTY(Transient) TObjectPtr<UVerticalBox>    HPVerticalBox;

    // HP バー (7 部位)
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_Head;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_Chest;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_Abdomen;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_LeftArm;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_RightArm;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_LeftLeg;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_RightLeg;

    // Sprint 19: 防具耐久度バー (Head/Chest/Abdomen)
    UPROPERTY(Transient) TObjectPtr<UVerticalBox>  ArmorVerticalBox;
    UPROPERTY(Transient) TObjectPtr<UProgressBar>  PB_Armor_Head;
    UPROPERTY(Transient) TObjectPtr<UProgressBar>  PB_Armor_Chest;
    UPROPERTY(Transient) TObjectPtr<UProgressBar>  PB_Armor_Abdomen;

    // Sprint 20: 状態異常アイコン (各 StatusType 1 つ)
    UPROPERTY(Transient) TObjectPtr<UHorizontalBox> StatusIconBox;
    UPROPERTY(Transient) TObjectPtr<UBorder>        StatusIcon_Bleed;
    UPROPERTY(Transient) TObjectPtr<UBorder>        StatusIcon_Fracture;
    UPROPERTY(Transient) TObjectPtr<UBorder>        StatusIcon_Pain;
    UPROPERTY(Transient) TObjectPtr<UBorder>        StatusIcon_Dehydration;
    UPROPERTY(Transient) TObjectPtr<UBorder>        StatusIcon_Fatigue;
    UPROPERTY(Transient) TObjectPtr<UBorder>        StatusIcon_Neuro;

private:
    UPROPERTY()
    TWeakObjectPtr<APlayerCharacterBase> OwningPlayer;

    void BuildLayout();
    UProgressBar* CreateBodyPartBar(UVerticalBox* Parent, FName Name);
    UProgressBar* CreateArmorBar(UVerticalBox* Parent, FName Name);
    UBorder*      CreateStatusIcon(UHorizontalBox* Parent, FName Name);
    UBorder*      GetStatusIconForType(EBXStatusType Type) const;
    UProgressBar* GetBarForBodyPart(EBXBodyPart BodyPart) const;
    UProgressBar* GetArmorBarForBodyPart(EBXBodyPart BodyPart) const;
    static FLinearColor ColorForRatio(float Ratio);
    static FLinearColor ArmorColorForRatio(float Ratio);
    static FLinearColor StatusColorForType(EBXStatusType Type);
};
