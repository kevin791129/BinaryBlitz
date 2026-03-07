// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Unit/UnitTypes.h"
#include "BinaryBlitzUnitPool.generated.h"

struct FUnitPoolEntry
{
public:
	AActor* Unit;
	bool bInUse;

	FUnitPoolEntry() : Unit(nullptr), bInUse(false) { }
	FUnitPoolEntry(AActor* InUnit, bool bInInUse) : Unit(InUnit), bInUse(bInInUse) { }
};

UCLASS(Blueprintable)
class BINARYBLITZ_API ABinaryBlitzUnitPool : public AActor
{
	GENERATED_BODY()

public:
	ABinaryBlitzUnitPool();

	UFUNCTION(BlueprintCallable)
	AActor* AquireActor(EFaction Faction, EUnitType Type, const FVector& Position);

	UFUNCTION(BlueprintCallable)
	void ReleaseActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void PreWarm();

	UFUNCTION(BlueprintCallable)
	void ClearPool();

	UFUNCTION(BlueprintCallable)
	static ABinaryBlitzUnitPool* GetInstance() { return Instance; }

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

	AActor* SpawnNewPooled(EFaction Faction, EUnitType Type);

	void ActivateActor(AActor* Actor, const FVector& Position, const FRotator& Rotation);

	void DeactivateActor(AActor* Actor);

protected:
	TMap<EFaction, TMap<EUnitType, TArray<FUnitPoolEntry>>> UnitPool;
	TMap<EFaction, TMap<EUnitType, FName>> UnitRowName;


	UPROPERTY(EditAnywhere)
	FVector InactivateLocation;

	UPROPERTY(EditDefaultsOnly, Category = Spawn)
	FVector SpawnOffset;

	UPROPERTY(EditDefaultsOnly, Category = Spawn)
	FRotator GoodSpawnRotation;

	UPROPERTY(EditDefaultsOnly, Category = Spawn)
	FRotator EvilSpawnRotation;

private:
	bool bWarmed;

	static ABinaryBlitzUnitPool* Instance;
};