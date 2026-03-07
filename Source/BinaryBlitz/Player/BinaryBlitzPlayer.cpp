// Fill out your copyright notice in the Description page of Project Settings.

#include "BinaryBlitzPlayer.h"
#include "GameFramework/FloatingPawnMovement.h"
/* Input */
#include "EnhancedInputComponent.h"

ABinaryBlitzPlayer::ABinaryBlitzPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABinaryBlitzPlayer::BeginPlay()
{
	Super::BeginPlay();

	ClampToBounds();
	ApplyMoveSpeed();
}

void ABinaryBlitzPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent))
	{
		Input->BindAction(AdjustSpeedAction, ETriggerEvent::Triggered, this, &ABinaryBlitzPlayer::OnAdjustSpeed);
	}
}
 
void ABinaryBlitzPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

    ClampToBounds();
}

void ABinaryBlitzPlayer::ClampToBounds()
{
    FVector Location = GetActorLocation();

    Location.X = FMath::Clamp(Location.X, BoxMin.X, BoxMax.X);
    Location.Y = FMath::Clamp(Location.Y, BoxMin.Y, BoxMax.Y);
    Location.Z = FMath::Clamp(Location.Z, BoxMin.Z, BoxMax.Z);

    SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
}

void ABinaryBlitzPlayer::ApplyMoveSpeed()
{
	if (UFloatingPawnMovement* Move = Cast<UFloatingPawnMovement>(GetMovementComponent()))
	{
		Move->MaxSpeed = CurrentMoveSpeed;
	}
}

void ABinaryBlitzPlayer::OnAdjustSpeed(const FInputActionValue& Value)
{
	const float WheelValue = Value.Get<float>();
	if (FMath::IsNearlyZero(WheelValue))
	{
		return;
	}

	CurrentMoveSpeed = FMath::Clamp(
		CurrentMoveSpeed + WheelValue * SpeedStep,
		MinMoveSpeed,
		MaxMoveSpeed
	);

	ApplyMoveSpeed();
}

