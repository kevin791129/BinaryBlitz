// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class EMapRegion : uint8
{
	GoodBase		UMETA(DisplayName = "Good Base"),
	GoodLeft		UMETA(DisplayName = "Good Left"),
	GoodRight		UMETA(DisplayName = "Good Right"),
	GoodMid			UMETA(DisplayName = "Good Middle"),
	EvilBase		UMETA(DisplayName = "Evil Base"),
	EvilLeft		UMETA(DisplayName = "Evil Left"),
	EvilMid			UMETA(DisplayName = "Evil Middle"),
	EvilRight		UMETA(DisplayName = "Evil Right"),
	Unknown			UMETA(Hidden)
};
ENUM_RANGE_BY_FIRST_AND_LAST(EMapRegion, EMapRegion::GoodBase, EMapRegion::EvilRight);
