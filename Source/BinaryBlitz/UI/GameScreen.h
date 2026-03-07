// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "../Unit/UnitTypes.h"
#include "GameScreen.generated.h"

class UButton;
class UTextBlock;

UCLASS(Abstract, HideDropdown)
class BINARYBLITZ_API UGameScreen : public UUserWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnTypeChanged, EUnitType, Type);
	
public:
	UFUNCTION(BlueprintCallable)
	void TrySpawnUnit(const FVector& Position);

public:
	UPROPERTY(BlueprintAssignable)
	FOnSpawnTypeChanged OnSpawnTypeChanged;

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface

	UFUNCTION()
	void OnSmallUnitBtnClicked();
	UFUNCTION()
	void OnMediumUnitBtnClicked();
	UFUNCTION()
	void OnLargeUnitBtnClicked();

	void ToggleUnitButton(EUnitType Type, bool bEnable);

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* SmallBtn;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SmallText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SmallCost;
	UPROPERTY(meta = (BindWidget))
	UButton* MediumBtn;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MediumText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MediumCost;
	UPROPERTY(meta = (BindWidget))
	UButton* LargeBtn;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LargeText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LargeCost;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CoinCount;

	UPROPERTY(EditAnywhere, Category = Style)
	FLinearColor EnableColor;
	UPROPERTY(EditAnywhere, Category = Style)
	FLinearColor DisableColor;

	UPROPERTY(EditAnywhere, Category = Style)
	FLinearColor SelectColor;
	UPROPERTY(EditAnywhere, Category = Style)
	FLinearColor DeselectColor;

	UPROPERTY(Transient)
	TMap<EUnitType, int> UnitCostMap;

	UPROPERTY(Transient)
	EUnitType SpawnType;
};
