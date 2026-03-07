// Fill out your copyright notice in the Description page of Project Settings.

#include "BinaryBlitzPlayerController.h"
/* Widgets */
#include "../UI/GameScreen.h"
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

	if (SpawnIndicatorClass)
	{
		SpawnIndicator = GetWorld()->SpawnActor(SpawnIndicatorClass);
		SpawnIndicator->SetActorHiddenInGame(true);
	}

	if (GameScreenClass)
	{
		GameScreen = CreateWidget<UGameScreen>(this, GameScreenClass, FName("GameScreen"));
		if (GameScreen)
			GameScreen->AddToViewport();
	}
}
 
void ABinaryBlitzPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ABinaryBlitzPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	bValidSpawn = false;

	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			FNavLocation NavLocation;

			if (NavSys->ProjectPointToNavigation(HitResult.Location, NavLocation, FVector(100.0f)))
			{
				bValidSpawn = true;
				SpawnPoint = NavLocation.Location;
			}
		}
	}

	if (IsValid(SpawnIndicator))
	{
		SpawnIndicator->SetActorHiddenInGame(!bValidSpawn);
		if (bValidSpawn)
			SpawnIndicator->SetActorLocation(SpawnPoint);
	}
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
	if (bValidSpawn && GameScreen)
	{
		GameScreen->TrySpawnUnit(SpawnPoint);
	}
}