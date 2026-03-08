// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "StartScreen.generated.h"

class UWidgetSwitcher;
class UButton;

UCLASS(Abstract, HideDropdown)
class BINARYBLITZ_API UStartScreen : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	//~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	UFUNCTION()
	void OnStartBtnClicked();
	UFUNCTION()
	void OnQuitBtnClicked();
	UFUNCTION()
	void OnCreditsBtnClicked();
	UFUNCTION()
	void OnTutorialBtnClicked();
	UFUNCTION()
	void OnBackBtnClicked();
	

	void ChangePage(int PageNum);

protected:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* PageSwitcher;
	UPROPERTY(meta = (BindWidget))
	UButton* StartBtn;
	UPROPERTY(meta = (BindWidget))
	UButton* QuitBtn;
	UPROPERTY(meta = (BindWidget))
	UButton* CreditsBtn;
	UPROPERTY(meta = (BindWidget))
	UButton* TutorialBtn;
	UPROPERTY(meta = (BindWidget))
	UButton* BackBtn;
};
