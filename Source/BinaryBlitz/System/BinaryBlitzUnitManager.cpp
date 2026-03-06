// Fill out your copyright notice in the Description page of Project Settings.

#include "BinaryBlitzUnitManager.h"
/* Unit */
#include "../Unit/UnitStats.h"
#include "../Unit/UnitBase.h"
#include "../Unit/UnitMovementController.h"
/* Other */
#include "Kismet/GameplayStatics.h"
#include "../BinaryBlitz.h"

ABinaryBlitzUnitManager* ABinaryBlitzUnitManager::Instance = nullptr;

ABinaryBlitzUnitManager::ABinaryBlitzUnitManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABinaryBlitzUnitManager::BeginPlay()
{
	Super::BeginPlay();

	if (!Instance)
	{
		Instance = this;
	}
	else
	{
		UE_LOG(LogBinaryBlitz, Warning, TEXT("Creating duplicate unit manager actor."))
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("GoodBase"), FoundActors);
	if (FoundActors.Num() > 0)
	{
		GoodBase = Cast<AUnitBase>(FoundActors[0]);
	}
	if (!IsValid(GoodBase))
	{
		UE_LOG(LogBinaryBlitz, Error, TEXT("No good base found."))
	}

	FoundActors.Empty();
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("EvilBase"), FoundActors);
	if (FoundActors.Num() > 0)
	{
		EvilBase = Cast<AUnitBase>(FoundActors[0]);
	}
	if (!IsValid(EvilBase))
	{
		UE_LOG(LogBinaryBlitz, Error, TEXT("No evil base found."))
	}
}

void ABinaryBlitzUnitManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Instance = nullptr;

	Super::EndPlay(EndPlayReason);
}

void ABinaryBlitzUnitManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateTimer += DeltaSeconds;
	if (UpdateTimer >= UpdateInterval)
	{
		UpdateTimer = 0.0f;
		UpdateInternal();
	}
}

void ABinaryBlitzUnitManager::RegisterUnit(AUnitBase* Unit)
{
	if (!IsValid(Unit))
		return;

	if (Unit->GetFaction() == EFaction::Good)
		GoodUnits.AddUnique(Unit);
	else
		EvilUnits.AddUnique(Unit);
}

void ABinaryBlitzUnitManager::UnregisterUnit(AUnitBase* Unit)
{
	if (!IsValid(Unit))
		return;

	if (Unit->GetFaction() == EFaction::Good)
		GoodUnits.Remove(Unit);
	else
		EvilUnits.Remove(Unit);
}

AUnitBase* ABinaryBlitzUnitManager::FindClosestEnemy(AUnitBase* Unit)
{
	if (!IsValid(Unit))
		return nullptr;

	const TArray<AUnitBase*>& Opponents = (Unit->GetFaction() == EFaction::Good) ? EvilUnits : GoodUnits;
	AUnitBase* BestTarget = (Unit->GetFaction() == EFaction::Good) ? EvilBase : GoodBase;;
	float BestDistSq = FVector::DistSquared(Unit->GetActorLocation(), BestTarget->GetActorLocation());

	for (AUnitBase* Candidate : Opponents)
	{
		if (!IsValid(Candidate) || !Candidate->IsAlive())
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(Unit->GetActorLocation(), Candidate->GetActorLocation());

		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestTarget = Candidate;
		}
	}

	return BestTarget;
}
UE_DISABLE_OPTIMIZATION
void ABinaryBlitzUnitManager::UpdateInternal()
{
	for (AUnitBase* Unit : GoodUnits)
	{
		if (!IsValid(Unit) || !Unit->IsAlive())
		{
			continue;
		}

		AUnitBase* Target = FindClosestEnemy(Unit);
		Unit->SetTarget(Target);

		if (AUnitMovementController* AI = Cast<AUnitMovementController>(Unit->GetController()))
		{
			const float DistSq = FVector::DistSquared(Unit->GetActorLocation(), Target->GetActorLocation());

			if (DistSq <= FMath::Square(Unit->GetAttackRange()))
			{
				AI->StopUnitMovement();
			}
			else
			{
				AI->MoveToEnemy(Target, Unit->GetAttackRange() * MovementThreshold);
			}
		}
	}


	auto UpdateSide = [this](AUnitBase* Base, const TArray<AUnitBase*>& Units)
		{
			AUnitBase* Target = FindClosestEnemy(Base);
			Base->SetTarget(Target);

			for (AUnitBase* Unit : Units)
			{
				if (!IsValid(Unit) || !Unit->IsAlive())
				{
					continue;
				}

				Target = FindClosestEnemy(Unit);
				Unit->SetTarget(Target);

				if (AUnitMovementController* AI = Cast<AUnitMovementController>(Unit->GetController()))
				{
					const float DistSq = FVector::DistSquared(Unit->GetActorLocation(), Target->GetActorLocation());

					if (DistSq <= FMath::Square(Unit->GetAttackRange()))
					{
						AI->StopUnitMovement();
					}
					else
					{
						AI->MoveToEnemy(Target, Unit->GetAttackRange() * MovementThreshold);
					}
				}
			}


		};

	//UpdateSide(GoodBase, GoodUnits);
	UpdateSide(EvilBase, EvilUnits);
}
UE_ENABLE_OPTIMIZATION