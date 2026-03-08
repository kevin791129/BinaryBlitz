// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Unit/UnitTypes.h"
#include "BinaryBlitzGameState.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "Game State"))
enum class EGameState : uint8
{
	WaitingStart,
	InProgress,
	Over
};

UCLASS(meta = (DisplayName = "Binary Blitz Game State"))

class ABinaryBlitzGameState : public AGameStateBase
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EGameState, State);

public:
	EGameState GetGameState() const { return State; }

	void StartGame();

	UFUNCTION(BlueprintCallable)
	void EndGame(bool bInGoodWin);

	bool GetGoodWin() const { return bGoodWin; }

	double GetGameTime() const { return GameEndTime - GameStartTime; }

	int GetMoney(EFaction Faction) const;

	bool PurchaseUnit(EFaction Faction, int Cost);

	void CollectMoney(EFaction Faction, int Amount);

public:
	UPROPERTY(BlueprintAssignable)
	FOnGameStateChanged OnGameStateChanged;

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Good, meta = (DisplayName = "Coins"))
	int GoodMoney = 1000;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Evil, meta = (DisplayName = "Coins"))
	int EvilMoney = 1000;

	EGameState State = EGameState::WaitingStart;

	double GameStartTime;
	double GameEndTime;

	bool bGoodWin = false;
};