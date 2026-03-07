// Fill out your copyright notice in the Description page of Project Settings.

#include "GameScreen.h"
#include "../Unit/UnitStats.h"
/* Components */
#include "Components/TextBlock.h"
#include "Components/Button.h"
/* Managers */
#include "../System/BinaryBlitzUnitPool.h"
#include "../BinaryBlitzGameState.h"
#include "../BinaryBlitzGameInstance.h"

void UGameScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OnSmallUnitBtnClicked();

	SmallBtn->OnClicked.AddDynamic(this, &UGameScreen::OnSmallUnitBtnClicked);
	MediumBtn->OnClicked.AddDynamic(this, &UGameScreen::OnMediumUnitBtnClicked);
	LargeBtn->OnClicked.AddDynamic(this, &UGameScreen::OnLargeUnitBtnClicked);

	UBinaryBlitzGameInstance* GameInstance = Cast<UBinaryBlitzGameInstance>(GetGameInstance());
	if (!GameInstance || !GameInstance->GetUnitDataTable())
		return;

	for (const auto& TableRow : GameInstance->GetUnitDataTable()->GetRowMap())
	{
		if (const FUnitTableRow* TempUnitTableRow = reinterpret_cast<const FUnitTableRow*>(TableRow.Value))
		{
			if (TempUnitTableRow->Faction == EFaction::Evil || TempUnitTableRow->Type == EUnitType::Base)
				continue;

			int TempCost = static_cast<int>(TempUnitTableRow->Stats.Cost);
			UnitCostMap.Add(TempUnitTableRow->Type, TempCost);

			switch (TempUnitTableRow->Type)
			{
			case EUnitType::Small:
				SmallText->SetText(FText::FromString(TempUnitTableRow->Name));
				SmallCost->SetText(FText::FromString(FString::Printf(TEXT("$%d"), TempCost)));
				break;
			case EUnitType::Medium:
				MediumText->SetText(FText::FromString(TempUnitTableRow->Name));
				MediumCost->SetText(FText::FromString(FString::Printf(TEXT("$%d"), TempCost)));
				break;
			case EUnitType::Large:
				LargeText->SetText(FText::FromString(TempUnitTableRow->Name));
				LargeCost->SetText(FText::FromString(FString::Printf(TEXT("$%d"), TempCost)));
				break;
			}
		}
	}
}

void UGameScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (!GetWorld())
		return;

	auto UpdateButtons = [this](ABinaryBlitzGameState* GameState, EUnitType Type) {
			if (!UnitCostMap.Contains(Type))
				return;

			ToggleUnitButton(Type, GameState->GetMoney(EFaction::Good) >= UnitCostMap[Type]);
		};

	if (ABinaryBlitzGameState* GameState = Cast<ABinaryBlitzGameState>(GetWorld()->GetGameState()))
	{
		UpdateButtons(GameState, EUnitType::Small);
		UpdateButtons(GameState, EUnitType::Medium);
		UpdateButtons(GameState, EUnitType::Large);

		const int CurrentMoney = GameState->GetMoney(EFaction::Good);
		CoinCount->SetText(FText::FromString(FString::Printf(TEXT("Cash: $%d"), CurrentMoney)));
	}
}

void UGameScreen::TrySpawnUnit(const FVector& Position)
{
	if (!GetWorld())
		return;

	if (ABinaryBlitzGameState* GameState = Cast<ABinaryBlitzGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->PurchaseUnit(EFaction::Good, UnitCostMap[SpawnType]))
		{
			ABinaryBlitzUnitPool::GetInstance()->AquireActor(EFaction::Good, SpawnType, Position);		
		}
	}
}

void UGameScreen::OnSmallUnitBtnClicked()
{
	SpawnType = EUnitType::Small;
	SmallBtn->SetBackgroundColor(SelectColor);
	MediumBtn->SetBackgroundColor(DeselectColor);
	LargeBtn->SetBackgroundColor(DeselectColor);
}

void UGameScreen::OnMediumUnitBtnClicked()
{
	SpawnType = EUnitType::Medium;
	SmallBtn->SetBackgroundColor(DeselectColor);
	MediumBtn->SetBackgroundColor(SelectColor);
	LargeBtn->SetBackgroundColor(DeselectColor);
}

void UGameScreen::OnLargeUnitBtnClicked()
{
	SpawnType = EUnitType::Large;
	SmallBtn->SetBackgroundColor(DeselectColor);
	MediumBtn->SetBackgroundColor(DeselectColor);
	LargeBtn->SetBackgroundColor(SelectColor);
}

void UGameScreen::ToggleUnitButton(EUnitType Type, bool bEnable)
{
	switch (Type)
	{
	case EUnitType::Small:
		SmallBtn->SetIsEnabled(bEnable);
		SmallText->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		SmallCost->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		break;
	case EUnitType::Medium:
		MediumBtn->SetIsEnabled(bEnable);
		MediumText->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		MediumCost->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		break;
	case EUnitType::Large:
		LargeBtn->SetIsEnabled(bEnable);
		LargeText->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		LargeCost->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		break;
	}
}
