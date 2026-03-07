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
	/** Updates Character Movement : Rotation Rate (default 360 degree/s) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Rotation Speed"))
	float RotateSpeed;
	/** Damage per attack */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Damage"))
	float ATK;
	/** Cooldown time between attacks (second) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Attack Speed"))
	float Cooldown;
	/** Attack range (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Attack Range"))
	float Range;
	/** Attack facing dot product (keep around 0.95) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Attack Facing (Dot)"))
	float FacingDot;
	/** Cost to deploy unit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit Stats", meta = (DisplayName = "Cost"))
	float Cost;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Enemy AI Factors"))
struct FEnemyAIFactors
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI Factors", meta = (DisplayName = "Offence Score"))
	float OffenceScore;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI Factors", meta = (DisplayName = "Defence Score"))
	float DefenceScore;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy AI Factors", meta = (DisplayName = "Bias"))
	float Bias;
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
	/** Enemy AI factors */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Enemy AI Factors"))
	FEnemyAIFactors AIFactors;
	/** Object pool init number */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Pool Num"))
	int InitNum;
};