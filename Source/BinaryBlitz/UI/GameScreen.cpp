// Fill out your copyright notice in the Description page of Project Settings.

#include "GameScreen.h"
#include "../Unit/UnitStats.h"
#include "../Unit/UnitBase.h"
/* Components */
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
/* Managers */
#include "../System/BinaryBlitzUnitPool.h"
#include "../BinaryBlitzGameState.h"
#include "../BinaryBlitzGameInstance.h"
/* Other */
#include "Kismet/GameplayStatics.h"
#include "../BinaryBlitz.h"
UE_DISABLE_OPTIMIZATION
void UGameScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SmallBtn->OnClicked.AddDynamic(this, &UGameScreen::OnSmallUnitBtnClicked);
	MediumBtn->OnClicked.AddDynamic(this, &UGameScreen::OnMediumUnitBtnClicked);
	LargeBtn->OnClicked.AddDynamic(this, &UGameScreen::OnLargeUnitBtnClicked);
	FlyingBtn->OnClicked.AddDynamic(this, &UGameScreen::OnFlyingUnitBtnClicked);
	TowerBtn->OnClicked.AddDynamic(this, &UGameScreen::OnTowerUnitBtnClicked);
	FactoryBtn->OnClicked.AddDynamic(this, &UGameScreen::OnFactoryUnitBtnClicked);

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

			UnitCooldownMap.Add(TempUnitTableRow->Type, TempUnitTableRow->Stats.SpawnCooldown);

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
			case EUnitType::Flying:
				FlyingText->SetText(FText::FromString(TempUnitTableRow->Name));
				FlyingCost->SetText(FText::FromString(FString::Printf(TEXT("$%d"), TempCost)));
				break;
			case EUnitType::Tower:
				TowerText->SetText(FText::FromString(TempUnitTableRow->Name));
				TowerCost->SetText(FText::FromString(FString::Printf(TEXT("$%d"), TempCost)));
				break;
			case EUnitType::Factory:
				FactoryText->SetText(FText::FromString(TempUnitTableRow->Name));
				FactoryCost->SetText(FText::FromString(FString::Printf(TEXT("$%d"), TempCost)));
				break;
			}
		}
	}

	if (CooldownTintMat)
	{
		CooldownTintDynamic = UMaterialInstanceDynamic::Create(CooldownTintMat, this);
		if (CooldownTintDynamic)
		{
			SmallCooldownTint->SetBrushFromMaterial(CooldownTintDynamic);
			MediumCooldownTint->SetBrushFromMaterial(CooldownTintDynamic);
			LargeCooldownTint->SetBrushFromMaterial(CooldownTintDynamic);
			FlyingCooldownTint->SetBrushFromMaterial(CooldownTintDynamic);
			TowerCooldownTint->SetBrushFromMaterial(CooldownTintDynamic);
			FactoryCooldownTint->SetBrushFromMaterial(CooldownTintDynamic);
		}
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("GoodBase"), FoundActors);
	if (FoundActors.Num() > 0)
	{
		GoodBase = Cast<AUnitBase>(FoundActors[0]);
	}
	if (!IsValid(GoodBase))
	{
		UE_LOG(LogBinaryBlitz, Error, TEXT("No good base found."))
	}

	FoundActors.Empty();
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("EvilBase"), FoundActors);
	if (FoundActors.Num() > 0)
	{
		EvilBase = Cast<AUnitBase>(FoundActors[0]);
	}
	if (!IsValid(EvilBase))
	{
		UE_LOG(LogBinaryBlitz, Error, TEXT("No evil base found."))
	}
}

void UGameScreen::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsDesignTime())
		return;

	if (OnSpawnTypeChanged.IsBound() && UnitCostMap.Contains(SpawnType))
	{
		OnSmallUnitBtnClicked();
	}
}

void UGameScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

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
		UpdateButtons(GameState, EUnitType::Flying);
		UpdateButtons(GameState, EUnitType::Tower);
		UpdateButtons(GameState, EUnitType::Factory);

		const int CurrentMoney = GameState->GetMoney(EFaction::Good);
		CoinCount->SetText(FText::FromString(FString::Printf(TEXT("$%d"), CurrentMoney)));
	}

	if (bCooldown)
	{
		CooldownTimer += InDeltaTime;

		if (CooldownTimer >= CooldownTargetTime)
		{
			ToggleCooldownTints(false);
		}
		else
		{
			if (IsValid(CooldownTintDynamic))
				CooldownTintDynamic->SetScalarParameterValue(FName("Time"), CooldownTimer);
		}
	}

	if (IsValid(GoodBase))
	{
		GoodHP->SetPercent(GoodBase->GetHealthPercent());
	}
	if (IsValid(EvilBase))
	{
		EvilHP->SetPercent(EvilBase->GetHealthPercent());
	}
}

bool UGameScreen::TrySpawnUnit(const FVector& Position)
{
	if (!UnitCostMap.Contains(SpawnType))
		return false;

	if (!GetWorld())
		return false;

	CooldownTargetTime = UnitCooldownMap[SpawnType];
	if (IsValid(CooldownTintDynamic))
		CooldownTintDynamic->SetScalarParameterValue(FName("CooldownTime"), CooldownTargetTime);
	ToggleCooldownTints(true);

	
	if (ABinaryBlitzGameState* GameState = Cast<ABinaryBlitzGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->PurchaseUnit(EFaction::Good, UnitCostMap[SpawnType]))
		{
			AActor* SpawnedUnit = ABinaryBlitzUnitPool::GetInstance()->AquireActor(EFaction::Good, SpawnType, Position);
			return SpawnedUnit != nullptr;
		}
	}

	return false;
}

void UGameScreen::OnSmallUnitBtnClicked()
{
	SpawnType = EUnitType::Small;
	OnSpawnTypeChanged.Broadcast(SpawnType, UnitCostMap[SpawnType]);
	SmallBtn->SetBackgroundColor(SelectColor);
	MediumBtn->SetBackgroundColor(DeselectColor);
	LargeBtn->SetBackgroundColor(DeselectColor);
	FlyingBtn->SetBackgroundColor(DeselectColor);
	TowerBtn->SetBackgroundColor(DeselectColor);
	FactoryBtn->SetBackgroundColor(DeselectColor);
}

void UGameScreen::OnMediumUnitBtnClicked()
{
	SpawnType = EUnitType::Medium;
	OnSpawnTypeChanged.Broadcast(SpawnType, UnitCostMap[SpawnType]);
	SmallBtn->SetBackgroundColor(DeselectColor);
	MediumBtn->SetBackgroundColor(SelectColor);
	LargeBtn->SetBackgroundColor(DeselectColor);
	FlyingBtn->SetBackgroundColor(DeselectColor);
	TowerBtn->SetBackgroundColor(DeselectColor);
	FactoryBtn->SetBackgroundColor(DeselectColor);
}

void UGameScreen::OnLargeUnitBtnClicked()
{
	SpawnType = EUnitType::Large;
	OnSpawnTypeChanged.Broadcast(SpawnType, UnitCostMap[SpawnType]);
	SmallBtn->SetBackgroundColor(DeselectColor);
	MediumBtn->SetBackgroundColor(DeselectColor);
	LargeBtn->SetBackgroundColor(SelectColor);
	FlyingBtn->SetBackgroundColor(DeselectColor);
	TowerBtn->SetBackgroundColor(DeselectColor);
	FactoryBtn->SetBackgroundColor(DeselectColor);
}

void UGameScreen::OnFlyingUnitBtnClicked()
{
	SpawnType = EUnitType::Flying;
	OnSpawnTypeChanged.Broadcast(SpawnType, UnitCostMap[SpawnType]);
	SmallBtn->SetBackgroundColor(DeselectColor);
	MediumBtn->SetBackgroundColor(DeselectColor);
	LargeBtn->SetBackgroundColor(DeselectColor);
	FlyingBtn->SetBackgroundColor(SelectColor);
	TowerBtn->SetBackgroundColor(DeselectColor);
	FactoryBtn->SetBackgroundColor(DeselectColor);
}

void UGameScreen::OnTowerUnitBtnClicked()
{
	SpawnType = EUnitType::Tower;
	OnSpawnTypeChanged.Broadcast(SpawnType, UnitCostMap[SpawnType]);
	SmallBtn->SetBackgroundColor(DeselectColor);
	MediumBtn->SetBackgroundColor(DeselectColor);
	LargeBtn->SetBackgroundColor(DeselectColor);
	FlyingBtn->SetBackgroundColor(DeselectColor);
	TowerBtn->SetBackgroundColor(SelectColor);
	FactoryBtn->SetBackgroundColor(DeselectColor);
}

void UGameScreen::OnFactoryUnitBtnClicked()
{
	SpawnType = EUnitType::Factory;
	OnSpawnTypeChanged.Broadcast(SpawnType, UnitCostMap[SpawnType]);
	SmallBtn->SetBackgroundColor(DeselectColor);
	MediumBtn->SetBackgroundColor(DeselectColor);
	LargeBtn->SetBackgroundColor(DeselectColor);
	FlyingBtn->SetBackgroundColor(DeselectColor);
	TowerBtn->SetBackgroundColor(DeselectColor);
	FactoryBtn->SetBackgroundColor(SelectColor);
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
	case EUnitType::Flying:
		FlyingBtn->SetIsEnabled(bEnable);
		FlyingText->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		FlyingCost->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		break;
	case EUnitType::Tower:
		TowerBtn->SetIsEnabled(bEnable);
		TowerText->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		TowerCost->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		break;
	case EUnitType::Factory:
		FactoryBtn->SetIsEnabled(bEnable);
		FactoryText->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		FactoryCost->SetColorAndOpacity(bEnable ? EnableColor : DisableColor);
		break;
	}
}

void UGameScreen::ToggleCooldownTints(bool bEnable)
{
	const ESlateVisibility TintVisible = bEnable ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;

	SmallCooldownTint->SetVisibility(TintVisible);
	MediumCooldownTint->SetVisibility(TintVisible);
	LargeCooldownTint->SetVisibility(TintVisible);
	FlyingCooldownTint->SetVisibility(TintVisible);
	TowerCooldownTint->SetVisibility(TintVisible);
	FactoryCooldownTint->SetVisibility(TintVisible);

	if (bEnable)
	{
		bCooldown = true;
		CooldownTimer = 0.0f;
		if (IsValid(CooldownTintDynamic))
			CooldownTintDynamic->SetScalarParameterValue(FName("Time"), CooldownTimer);
	}
	else
	{
		bCooldown = false;
		OnSpawnCooldown.Broadcast();
	}
}
UE_ENABLE_OPTIMIZATION