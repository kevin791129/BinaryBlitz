// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BinaryBlitzConfig.generated.h"

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Binary Blitz Config"))
class BINARYBLITZ_API UBinaryBlitzConfig : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(Config, EditAnywhere, Category="Enemy AI")
    float DefendThreshold = 3.0f;

    UPROPERTY(Config, EditAnywhere, Category = "Enemy AI")
    float AttackThreshold = 3.0f;

    UPROPERTY(Config, EditAnywhere, Category = "Enemy AI")
    float BaseLowHealthPercent = 0.3f;

    UPROPERTY(Config, EditAnywhere, Category = "Enemy AI")
    float PriorityWeight = 3.0f;

    UPROPERTY(Config, EditAnywhere, Category = "Enemy AI")
    float OtherWeight = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category = "Enemy AI")
    float WaitWeight = 95.0f;

    UPROPERTY(Config, EditAnywhere, Category = "Balancing")
    int GoodStartingCash = 1000;

    UPROPERTY(Config, EditAnywhere, Category = "Balancing")
    int EvilStartingCash = 1000;

    /* Coins per second */
    UPROPERTY(Config, EditAnywhere, Category = "Balancing")
    int GoodPassiveIncome = 10.0f;

    /* Coins per second */
    UPROPERTY(Config, EditAnywhere, Category = "Balancing")
    int EvilPassiveIncome = 10.0f;

    UPROPERTY(Config, EditAnywhere, Category = "Balancing")
    float CoinDropMultiplier = 0.5f;

    UPROPERTY(Config, EditAnywhere, Category = "Manager|Movement")
    float MovementUpdateInterval = 0.2f;

    UPROPERTY(Config, EditAnywhere, Category = "Manager|Movement")
    float MoveToMultiplier = 0.9f;

    UPROPERTY(Config, EditAnywhere, Category = "Manager|AI")
    float AIUpdateInterval = 0.1f;
};
