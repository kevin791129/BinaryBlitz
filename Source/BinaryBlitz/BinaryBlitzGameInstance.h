// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "BinaryBlitzGameInstance.generated.h"

UCLASS(meta = (DisplayName = "Binary Blitz Game Instance"))

class UBinaryBlitzGameInstance : public UGameInstance 
{
	GENERATED_BODY()
	
public:
	UDataTable* GetUnitDataTable() const { return UnitTable; }

protected:
	UPROPERTY(EditAnywhere)
	UDataTable* UnitTable;
};