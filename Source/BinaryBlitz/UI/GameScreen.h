// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "../Unit/UnitTypes.h"
#include "GameScreen.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UProgressBar;
class AUnitBase;

UCLASS(Abstract, HideDropdown)
class BINARYBLITZ_API UGameScreen : public UUserWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpawnTypeChanged, EUnitType, Type, int, Cost);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawnCooldown);
	
public:
	UFUNCTION(BlueprintCallable)
	bool TrySpawnUnit(const FVector& Position);

public:
	UPROPERTY(BlueprintAssignable)
	FOnSpawnTypeChanged OnSpawnTypeChanged;

	UPROPERTY(BlueprintAssignable)
	FOnSpawnCooldown OnSpawnCooldown;

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface

	UFUNCTION()
	void OnSmallUnitBtnClicked();
	UFUNCTION()
	void OnMediumUnitBtnClicked();
	UFUNCTION()
	void OnLargeUnitBtnClicked();
	UFUNCTION()
	void OnFlyingUnitBtnClicked();
	UFUNCTION()
	void OnTowerUnitBtnClicked();
	UFUNCTION()
	void OnFactoryUnitBtnClicked();

	void ToggleUnitButton(EUnitType Type, bool bEnable);

	void ToggleCooldownTints(bool bEnable);

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* SmallBtn;
	UPROPERTY(meta = (BindWidget))
	UImage* SmallIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SmallText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SmallCost;
	UPROPERTY(meta = (BindWidget))
	UImage* SmallCooldownTint;
	UPROPERTY(meta = (BindWidget))
	UButton* MediumBtn;
	UPROPERTY(meta = (BindWidget))
	UImage* MediumIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MediumText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MediumCost;
	UPROPERTY(meta = (BindWidget))
	UImage* MediumCooldownTint;
	UPROPERTY(meta = (BindWidget))
	UButton* LargeBtn;
	UPROPERTY(meta = (BindWidget))
	UImage* LargeIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LargeText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LargeCost;
	UPROPERTY(meta = (BindWidget))
	UImage* LargeCooldownTint;
	UPROPERTY(meta = (BindWidget))
	UButton* FlyingBtn;
	UPROPERTY(meta = (BindWidget))
	UImage* FlyingIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FlyingText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FlyingCost;
	UPROPERTY(meta = (BindWidget))
	UImage* FlyingCooldownTint;
	UPROPERTY(meta = (BindWidget))
	UButton* TowerBtn;
	UPROPERTY(meta = (BindWidget))
	UImage* TowerIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TowerText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TowerCost;
	UPROPERTY(meta = (BindWidget))
	UImage* TowerCooldownTint;
	UPROPERTY(meta = (BindWidget))
	UButton* FactoryBtn;
	UPROPERTY(meta = (BindWidget))
	UImage* FactoryIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FactoryText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FactoryCost;
	UPROPERTY(meta = (BindWidget))
	UImage* FactoryCooldownTint;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CoinCount;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* GoodHP;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* EvilHP;

	UPROPERTY(EditAnywhere, Category = Style)
	FLinearColor EnableColor;
	UPROPERTY(EditAnywhere, Category = Style)
	FLinearColor DisableColor;

	UPROPERTY(EditAnywhere, Category = Style)
	FLinearColor SelectColor;
	UPROPERTY(EditAnywhere, Category = Style)
	FLinearColor DeselectColor;

	UPROPERTY(EditAnywhere, Category = Style)
	UMaterialInterface* CooldownTintMat;

	UPROPERTY(Transient)
	TMap<EUnitType, int> UnitCostMap;
	UPROPERTY(Transient)
	TMap<EUnitType, float> UnitCooldownMap;

	UPROPERTY(Transient)
	EUnitType SpawnType;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CooldownTintDynamic;

	UPROPERTY(Transient)
	AUnitBase* GoodBase;
	UPROPERTY(Transient)
	AUnitBase* EvilBase;

private:
	bool bCooldown = false;
	float CooldownTimer = 0.0f;
	float CooldownTargetTime = 0.0f;
};
