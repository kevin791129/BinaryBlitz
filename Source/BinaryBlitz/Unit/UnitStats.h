// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnitTypes.h"
#include "Engine/DataTable.h"
#include "UnitStats.generated.h"

class AUnitBase;

USTRUCT(BlueprintType, meta = (DisplayName = "Unit Stats"))
struct FUnitStats
{
	GENERATED_BODY()
	
public:	
	/** Total hit points */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Max HP"))
	float MaxHP;
	/** Updates Character Movement : Max Walk Speed (default 600 cm/s) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Movement Speed"))
	float Speed;
	/** Damage per attack */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Damage"))
	float ATK;
	/** Cooldown time between attacks (second) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Attack Speed"))
	float Cooldown;
	/** Attack range (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Attack Range"))
	float Range;
	/** Cost to deploy unit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Cost"))
	float Cost;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Unit Table Row"))
struct FUnitTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** BP class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Blueprint"))
	TSoftClassPtr<AUnitBase> Asset;
	/** Faction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Faction"))
	EFaction Faction;
	/** Type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Type"))
	EUnitType Type;
	/** Name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Name"))
	FString Name;
	/** Stats */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Stats"))
	FUnitStats Stats;
	/** Object pool init number */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Pool Num"))
	int InitNum;
};