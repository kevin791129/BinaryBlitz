// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BinaryBlitzUnitManager.generated.h"

class AUnitBase;

UCLASS(Blueprintable)
class BINARYBLITZ_API ABinaryBlitzUnitManager : public AActor
{
	GENERATED_BODY()

public:
	ABinaryBlitzUnitManager();

	//~ Begin AActor Interface
	virtual void Tick(float DeltaSeconds) override;
	//~ End AActor Interface

	UFUNCTION(BlueprintCallable)
	static ABinaryBlitzUnitManager* GetInstance() { return Instance; }

	UFUNCTION(BlueprintCallable)
	void RegisterUnit(AUnitBase* Unit);

	UFUNCTION(BlueprintCallable)
	void UnregisterUnit(AUnitBase* Unit);

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

	AUnitBase* FindClosestEnemy(AUnitBase* Unit);

	void UpdateInternal();

protected:
	UPROPERTY(Transient)
	TArray<AUnitBase*> GoodUnits;

	UPROPERTY(Transient)
	AUnitBase* GoodBase;

	UPROPERTY(Transient)
	TArray<AUnitBase*> EvilUnits;

	UPROPERTY(Transient)
	AUnitBase* EvilBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Update Interval"))
	float UpdateInterval = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Movement Threshold"))
	float MovementThreshold = 0.5f;

private:
	float UpdateTimer = 0.0f;

	static ABinaryBlitzUnitManager* Instance;
};