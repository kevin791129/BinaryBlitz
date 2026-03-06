// Fill out your copyright notice in the Description page of Project Settings.

#include "BinaryBlitzPlayerController.h"
#include "../System/BinaryBlitzUnitPool.h"
/* Input */
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
/* Other */
#include "NavigationSystem.h"
#include "../BinaryBlitz.h"

void ABinaryBlitzPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	bShowMouseCursor = true;
}
 
void ABinaryBlitzPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ABinaryBlitzPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent))
	{
		Input->BindAction(SpawnUnitAction, ETriggerEvent::Triggered, this, &ABinaryBlitzPlayerController::OnSpawnUnitAction);
	}
}

void ABinaryBlitzPlayerController::OnSpawnUnitAction()
{
	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			FNavLocation NavLocation;

			if (NavSys->ProjectPointToNavigation(HitResult.Location, NavLocation, FVector(100.0f)))
			{
				FVector SpawnPosition = NavLocation.Location;
				SpawnPosition.Z += 50.f;

				AActor* Actor = ABinaryBlitzUnitPool::GetInstance()->AquireActor(EFaction::Good, SpawnType, SpawnPosition);
			}
		}

	}
}