// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType, meta = (DisplayName = "Faction"))
enum class EFaction : uint8
{
	Good		UMETA(DisplayName = "Good"),
	Evil		UMETA(DisplayName = "Evil"),
};

UENUM(BlueprintType, meta = (DisplayName = "Unit Type"))
enum class EUnitType : uint8
{
	Base 		UMETA(DisplayName = "Base"),
	Small 		UMETA(DisplayName = "Small"),
	Medium 		UMETA(DisplayName = "Medium"),
	Large 		UMETA(DisplayName = "Large"),
	Flying 		UMETA(DisplayName = "Flying"),
	Tower 		UMETA(DisplayName = "Tower"),
	Factory		UMETA(DisplayName = "Factory"),
};