// Fill out your copyright notice in the Description page of Project Settings.

#include "EndScreen.h"
/* Components */
#include "Components/TextBlock.h"
#include "Components/Button.h"
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
        ResultText->SetText(FText::FromString("Good Victory"));
        const FString DisplayStr = FString::Printf(TEXT("You destroyed the evil base in %s."), *TimeStr);
        ResultText2->SetText(FText::FromString(DisplayStr));
    }
    else
    {
        ResultText->SetText(FText::FromString("Evil Victory"));
        const FString DisplayStr = FString::Printf(TEXT("You held the evil assult for %s."), *TimeStr);
        ResultText2->SetText(FText::FromString(DisplayStr));
    }
}

void UEndScreen::OnRestartBtnClicked()
{
    UWorld* World = GetWorld();
    if (!World) return;

    FName CurrentLevel = *UGameplayStatics::GetCurrentLevelName(World, true);
    UGameplayStatics::OpenLevel(World, CurrentLevel);
}