// Fill out your copyright notice in the Description page of Project Settings.

#include "RegionTrigger.h"
/* Components */
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#endif

ARegionTrigger::ARegionTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    Box = CreateDefaultSubobject<UBoxComponent>(TEXT("RegionBox"));
    RootComponent = Box;

    Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Box->SetCollisionResponseToAllChannels(ECR_Ignore);
    Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
    SpawnPoint->SetupAttachment(Box);

#if WITH_EDITORONLY_DATA
    ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPointVisual"));
    ArrowComponent->SetupAttachment(SpawnPoint);
    ArrowComponent->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    ArrowComponent->SetAbsolute(false, true, true);
#endif
}

bool ARegionTrigger::ContainsActor(AActor* Actor) const
{
    if (!IsValid(Box))
        return false;
    
    return Box->IsOverlappingActor(Actor);
}

FVector ARegionTrigger::GetSpawnLocation() const
{
    if (!IsValid(SpawnPoint))
        return FallbackSpawnLocation;

    return SpawnPoint->GetComponentLocation();
}