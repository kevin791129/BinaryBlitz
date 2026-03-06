// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Unit/UnitTypes.h"
#include "BinaryBlitzPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS(meta = (DisplayName = "Binary Blitz Player Controller"))
class BINARYBLITZ_API ABinaryBlitzPlayerController : public APlayerController
{
	GENERATED_BODY()

public:


protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

	//~ Begin APlayerController Interface
	virtual void SetupInputComponent() override;
	//~ End APlayerController Interface

	void OnSpawnUnitAction();

protected:
	/** Default mapping context */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (DisplayName = "Default Mapping Context"))
	UInputMappingContext* DefaultMappingContext;
	/** Spawn unit action */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (DisplayName = "Spawn Unit Action"))
	UInputAction* SpawnUnitAction;

	UPROPERTY(BlueprintReadWrite)
	EUnitType SpawnType = EUnitType::Small;
};