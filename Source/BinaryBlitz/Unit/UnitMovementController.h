// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UnitMovementController.generated.h"

class AUnitBase;

UCLASS(meta = (DisplayName = "Unit Movement Controller"))
class BINARYBLITZ_API AUnitMovementController : public AAIController
{
	GENERATED_BODY()

public:
	AUnitMovementController();

    UFUNCTION(BlueprintCallable)
    void MoveToEnemy(AUnitBase* Target, float AcceptanceRadius = 100.f);

    UFUNCTION(BlueprintCallable)
    void StopUnitMovement();
};