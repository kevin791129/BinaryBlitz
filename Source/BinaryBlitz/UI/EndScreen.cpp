// Fill out your copyright notice in the Description page of Project Settings.

#include "EndScreen.h"
/* Components */
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
/* Other */
#include "Kismet/GameplayStatics.h"
#include "Misc/Timespan.h"


void UEndScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

    RestartBtn->OnClicked.AddDynamic(this, &UEndScreen::OnRestartBtnClicked);
}

void UEndScreen::DisplayResult(bool bGoodWin, float Time)
{
    FTimespan Timespan = FTimespan::FromSeconds(Time);
    FString TimeStr = Timespan.ToString(TEXT("%h:%m:%s")).RightChop(1);

    if (bGoodWin)
    {
        const FString DisplayStr = FString::Printf(TEXT("You destroyed the evil base in %s."), *TimeStr);
        ResultText->SetText(FText::FromString(DisplayStr));
        VictoryScreen->SetVisibility(ESlateVisibility::Visible);
        LoseScreen->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        const FString DisplayStr = FString::Printf(TEXT("You held the evil assult for %s."), *TimeStr);
        ResultText->SetText(FText::FromString(DisplayStr));
        VictoryScreen->SetVisibility(ESlateVisibility::Hidden);
        LoseScreen->SetVisibility(ESlateVisibility::Visible);
    }
}

void UEndScreen::OnRestartBtnClicked()
{
    UWorld* World = GetWorld();
    if (!World) return;

    FName CurrentLevel = *UGameplayStatics::GetCurrentLevelName(World, true);
    UGameplayStatics::OpenLevel(World, CurrentLevel);
}