// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "EndScreen.generated.h"

class UButton;
class UTextBlock;
class UBorder;

UCLASS(Abstract, HideDropdown)
class BINARYBLITZ_API UEndScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	void DisplayResult(bool bGoodWin, float Time);
	
protected:
	//~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	UFUNCTION()
	void OnRestartBtnClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	UBorder* VictoryScreen;

	UPROPERTY(meta = (BindWidget))
	UBorder* LoseScreen;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResultText;

	UPROPERTY(meta = (BindWidget))
	UButton* RestartBtn;
};
