// Copyright (c) takumi2911 - BlackoutExfil
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/BXEnums.h"
#include "BXHUDWidget.generated.h"

class APlayerCharacterBase;
class UCanvasPanel;
class UVerticalBox;
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

protected:
    UPROPERTY(Transient) TObjectPtr<UCanvasPanel> RootCanvas;
    UPROPERTY(Transient) TObjectPtr<UBorder>      Crosshair;
    UPROPERTY(Transient) TObjectPtr<UTextBlock>   AmmoText;
    UPROPERTY(Transient) TObjectPtr<UVerticalBox> HPVerticalBox;

    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_Head;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_Chest;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_Abdomen;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_LeftArm;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_RightArm;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_LeftLeg;
    UPROPERTY(Transient) TObjectPtr<UProgressBar> PB_HP_RightLeg;

private:
    UPROPERTY()
    TWeakObjectPtr<APlayerCharacterBase> OwningPlayer;

    void BuildLayout();
    UProgressBar* CreateBodyPartBar(UVerticalBox* Parent, FName Name);
    UProgressBar* GetBarForBodyPart(EBXBodyPart BodyPart) const;
    static FLinearColor ColorForRatio(float Ratio);
};