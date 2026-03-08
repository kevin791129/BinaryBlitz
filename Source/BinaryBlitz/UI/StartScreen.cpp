// Fill out your copyright notice in the Description page of Project Settings.

#include "StartScreen.h"
/* Components */
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
/* Other */
#include "../BinaryBlitzGameState.h"
#include "Kismet/KismetSystemLibrary.h"

void UStartScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	StartBtn->OnClicked.AddDynamic(this, &UStartScreen::OnStartBtnClicked);
	QuitBtn->OnClicked.AddDynamic(this, &UStartScreen::OnQuitBtnClicked);
	CreditsBtn->OnClicked.AddDynamic(this, &UStartScreen::OnCreditsBtnClicked);
	TutorialBtn->OnClicked.AddDynamic(this, &UStartScreen::OnTutorialBtnClicked);
	BackBtn->OnClicked.AddDynamic(this, &UStartScreen::OnBackBtnClicked);
}

void UStartScreen::OnStartBtnClicked()
{
	if (ABinaryBlitzGameState* GameState = Cast<ABinaryBlitzGameState>(GetWorld()->GetGameState()))
	{
		GameState->StartGame();
	}
}

void UStartScreen::OnQuitBtnClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}

void UStartScreen::OnCreditsBtnClicked()
{
	ChangePage(1);
	StartBtn->SetVisibility(ESlateVisibility::Hidden);
	QuitBtn->SetVisibility(ESlateVisibility::Hidden);
	CreditsBtn->SetVisibility(ESlateVisibility::Hidden);
	TutorialBtn->SetVisibility(ESlateVisibility::Hidden);
	BackBtn->SetVisibility(ESlateVisibility::Visible);

}

void UStartScreen::OnTutorialBtnClicked()
{
	ChangePage(2);
	StartBtn->SetVisibility(ESlateVisibility::Hidden);
	QuitBtn->SetVisibility(ESlateVisibility::Hidden);
	CreditsBtn->SetVisibility(ESlateVisibility::Hidden);
	TutorialBtn->SetVisibility(ESlateVisibility::Hidden);
	BackBtn->SetVisibility(ESlateVisibility::Visible);
	BackBtn->SetVisibility(ESlateVisibility::Visible);
}

void UStartScreen::OnBackBtnClicked()
{
	ChangePage(0);
	StartBtn->SetVisibility(ESlateVisibility::Visible); 
	QuitBtn->SetVisibility(ESlateVisibility::Visible);
	CreditsBtn->SetVisibility(ESlateVisibility::Visible);
	TutorialBtn->SetVisibility(ESlateVisibility::Visible);
	BackBtn->SetVisibility(ESlateVisibility::Hidden);
	BackBtn->SetVisibility(ESlateVisibility::Hidden);
}

void UStartScreen::ChangePage(int PageNum)
{
	if (PageNum >= PageSwitcher->GetNumWidgets())
		return;
	PageSwitcher->SetActiveWidgetIndex(PageNum);
}