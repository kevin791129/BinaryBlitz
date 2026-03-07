// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitMovementController.h"
#include "UnitBase.h"
#include "Navigation/PathFollowingComponent.h"

AUnitMovementController::AUnitMovementController()
{
    bAttachToPawn = true;
}

void AUnitMovementController::MoveToEnemy(AUnitBase* Target, float AcceptanceRadius)
{
    if (!Target)
    {
        return;
    }

    FAIMoveRequest Request;
    Request.SetGoalActor(Target);
    Request.SetAcceptanceRadius(AcceptanceRadius);
    Request.SetUsePathfinding(true);
    Request.SetAllowPartialPath(true);

    MoveTo(Request);
}

void AUnitMovementController::MoveToGoalLocation(const FVector& GoalLocation, float AcceptanceRadius)
{
    FAIMoveRequest Request;
    Request.SetGoalLocation(GoalLocation);
    Request.SetAcceptanceRadius(AcceptanceRadius);
    Request.SetUsePathfinding(true);
    Request.SetAllowPartialPath(true);

    MoveTo(Request);
}

void AUnitMovementController::StopUnitMovement()
{
    StopMovement();
}

void AUnitMovementController::SetCombatFocus(AUnitBase* Target)
{
    if (!Target)
    {
        return;
    }

    SetFocus(Target, EAIFocusPriority::Gameplay);
}

void AUnitMovementController::ClearCombatFocus()
{
    ClearFocus(EAIFocusPriority::Gameplay);
}