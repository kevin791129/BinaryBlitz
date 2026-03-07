// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapRegion.h"
#include "../Unit/UnitTypes.h"
#include "EnemyAIManager.generated.h"

class AUnitBase;
class ARegionTrigger;

namespace ESpawnMode
{
	enum Type
	{
		Defend,
		Attack,
		Resource
	};
}

UCLASS(Blueprintable)
class BINARYBLITZ_API AEnemyAIManager : public AActor
{
	GENERATED_BODY()

public:
	AEnemyAIManager();

	//~ Begin AActor Interface
	virtual void Tick(float DeltaSeconds) override;
	//~ End AActor Interface

	UFUNCTION(BlueprintCallable)
	static AEnemyAIManager* GetInstance() { return Instance; }

protected:
	enum ESpawnAction
	{
		Spawn,
		Wait,
	};

	struct FSpawnAction
	{
		ESpawnAction Action;
		EMapRegion Region;

		/** Wait reply */
		FSpawnAction() : Action(ESpawnAction::Wait), Region(EMapRegion::Unknown) { }
		/** Spawn reply */
		FSpawnAction(EMapRegion InRegion) : Action(ESpawnAction::Spawn), Region(InRegion) { }
	};

	enum ESpawnResult
	{
		Spawned,
		InsufficientFunds,
	};

	struct FSpawnResult
	{
		ESpawnResult Result;
		AUnitBase* Unit;

		/** Failed reply */
		FSpawnResult() : Result(ESpawnResult::InsufficientFunds), Unit(nullptr) { }
		/** Success reply */
		FSpawnResult(AUnitBase* InUnit) : Result(ESpawnResult::Spawned), Unit(InUnit) { }
	};

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

	void UpdateInternal();

	void CalculateRegionScores();

	FSpawnAction DetermineAction();

	EUnitType DetermineUnitType();

	FSpawnResult TrySpawnUnit(EUnitType Type, const FVector& Position);

protected:

	UPROPERTY(Transient)
	TArray<ARegionTrigger*> RegionTriggers;
	UPROPERTY(Transient)
	TMap<EMapRegion, int> RegionTriggerIndex;

	UPROPERTY(Transient)
	TMap<EMapRegion, float> RegionThreatScore;
	UPROPERTY(Transient)
	TMap<EMapRegion, float> RegionAttackScore;

	UPROPERTY(Transient)
	TMap<EUnitType, int> UnitCostMap;

	float DefendThreshold;
	float AttackThreshold;
	float BaseLowHealthPercent;
	float PriorityWeight;
	float OtherWeight;
	float WaitWeight;
	float UpdateInterval = 0.2f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere)
	bool bDebugScores = false;
#endif

private:
	float UpdateTimer = 0.0f;

	static AEnemyAIManager* Instance;
};