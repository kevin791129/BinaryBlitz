// Fill out your copyright notice in the Description page of Project Settings.

#include "BinaryBlitzGameState.h"
#include "BinaryBlitzConfig.h"

void ABinaryBlitzGameState::BeginPlay()
{
	Super::BeginPlay();

	if (const UBinaryBlitzConfig* Config = GetDefault<UBinaryBlitzConfig>())
	{
		GoodMoney = Config->GoodStartingCash;
		EvilMoney = Config->EvilStartingCash;
	}
}

void ABinaryBlitzGameState::StartGame()
{
	GameStartTime = GetWorld()->GetTimeSeconds();
	State = EGameState::InProgress;
	OnGameStateChanged.Broadcast(State);
}

void ABinaryBlitzGameState::EndGame(bool bInGoodWin)
{
	bGoodWin = bInGoodWin;
	GameEndTime = GetWorld()->GetTimeSeconds();
	State = EGameState::Over;
	OnGameStateChanged.Broadcast(State);
}

int ABinaryBlitzGameState::GetMoney(EFaction Faction) const
{
	return Faction == EFaction::Good ? GoodMoney : EvilMoney;
}

bool ABinaryBlitzGameState::PurchaseUnit(EFaction Faction, int Cost)
{
	if (Faction == EFaction::Good)
	{
		if (GoodMoney >= Cost)
		{
			GoodMoney -= Cost;
			return true;
		}
	}
	else
	{
		if (EvilMoney >= Cost)
		{
			EvilMoney -= Cost;
			return true;
		}
	}
	return false;
}

void ABinaryBlitzGameState::CollectMoney(EFaction Faction, int Amount)
{
	if (Faction == EFaction::Good)
		GoodMoney += Amount;
	else
		EvilMoney += Amount;
}