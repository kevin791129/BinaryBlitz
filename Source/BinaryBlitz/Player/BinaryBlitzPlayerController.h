// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Unit/UnitTypes.h"
#include "../BinaryBlitzGameState.h"
#include "BinaryBlitzPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UStartScreen;
class UGameScreen;
class UEndScreen;

UCLASS(meta = (DisplayName = "Binary Blitz Player Controller"))
class BINARYBLITZ_API ABinaryBlitzPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//~ Begin AActor Interface
	virtual void Tick(float DeltaSeconds) override;
	//~ End AActor Interface

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

	//~ Begin APlayerController Interface
	virtual void SetupInputComponent() override;
	//~ End APlayerController Interface

	void OnSpawnUnitAction();

	bool IsOnCorrectSide(const FVector2D& Position) const;

	UFUNCTION()
	void OnGameStateChanged(EGameState State);

	UFUNCTION()
	void OnSpawnTypeChanged(EUnitType Type, int Cost);

	UFUNCTION()
	void OnSpawnCooldown();

protected:
	/** Default mapping context */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (DisplayName = "Default Mapping Context"))
	UInputMappingContext* DefaultMappingContext;
	/** Spawn unit action */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (DisplayName = "Spawn Unit Action"))
	UInputAction* SpawnUnitAction;

	UPROPERTY(BlueprintReadOnly, Category = Indicator)
	AActor* SpawnIndicator = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Indicator)
	TSubclassOf<AActor> SpawnIndicatorClass;
	UPROPERTY(EditAnywhere, Category = Indicator)
	TArray<FVector2D> DivisionPoints;
	
	UPROPERTY(BlueprintReadOnly, Category = UI)
	UStartScreen* StartScreen;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UStartScreen> StartScreenClass;
	UPROPERTY(BlueprintReadOnly, Category = UI)
	UGameScreen* GameScreen;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UGameScreen> GameScreenClass;
	UPROPERTY(BlueprintReadOnly, Category = UI)
	UEndScreen* EndScreen;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UEndScreen> EndScreenClass;

	

private:
	bool bValidSpawn = false;

	EUnitType SpawnType = EUnitType::Small;

	int UnitCost = 50;

	bool bSpawnCooldown = false;

	FVector SpawnPoint = FVector::ZeroVector;
};