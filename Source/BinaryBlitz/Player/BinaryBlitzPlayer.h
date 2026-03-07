// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "InputActionValue.h"
#include "BinaryBlitzPlayer.generated.h"

class UInputAction;

UCLASS(meta = (DisplayName = "Binary Blitz Player"))
class BINARYBLITZ_API ABinaryBlitzPlayer : public ADefaultPawn
{
	GENERATED_BODY()

public:
	ABinaryBlitzPlayer();

	//~ Begin AActor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	//~ End AActor Interface

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

    void ClampToBounds();
	void ApplyMoveSpeed();

	// Enhanced Input callback
	void OnAdjustSpeed(const struct FInputActionValue& Value);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds")
	FVector BoxMin = FVector(-10000.f, -10000.f, 1000.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds")
	FVector BoxMax = FVector(10000.f, 10000.f, 3000.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinMoveSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxMoveSpeed = 4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpeedStep = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CurrentMoveSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* AdjustSpeedAction;
};