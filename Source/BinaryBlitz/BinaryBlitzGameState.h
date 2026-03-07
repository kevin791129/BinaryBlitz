// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Unit/UnitTypes.h"
#include "BinaryBlitzGameState.generated.h"

UCLASS(meta = (DisplayName = "Binary Blitz Game State"))

class ABinaryBlitzGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	int GetMoney(EFaction Faction) const;

	bool PurchaseUnit(EFaction Faction, int Cost);

	void CollectMoney(EFaction Faction, int Amount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Good, meta = (DisplayName = "Coins"))
	int GoodMoney = 1000;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Evil, meta = (DisplayName = "Coins"))
	int EvilMoney = 1000;
};