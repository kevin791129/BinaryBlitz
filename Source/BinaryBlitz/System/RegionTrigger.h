// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapRegion.h"
#include "RegionTrigger.generated.h"

class USceneComponent;
class UBoxComponent;
class UArrowComponent;

UCLASS(Blueprintable)
class BINARYBLITZ_API ARegionTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ARegionTrigger();

	UFUNCTION(BlueprintCallable)
	EMapRegion GetRegion() const { return Region; }

	UFUNCTION(BlueprintCallable)
	bool ContainsActor(AActor* Actor) const;

	UFUNCTION(BlueprintCallable)
	FVector GetSpawnLocation() const;

protected:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapRegion Region;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FallbackSpawnLocation;

#if WITH_EDITORONLY_DATA
	/** Visual reference of the spawn point*/
	UPROPERTY()
	UArrowComponent* ArrowComponent;
#endif
};
