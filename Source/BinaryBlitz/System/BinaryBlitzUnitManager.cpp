// Fill out your copyright notice in the Description page of Project Settings.

#include "BinaryBlitzUnitManager.h"
/* Unit */
#include "../Unit/UnitStats.h"
#include "../Unit/UnitBase.h"
#include "../Unit/UnitMovementController.h"
/* Other */
#include "Kismet/GameplayStatics.h"
#include "../BinaryBlitzConfig.h"
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

	if (ABinaryBlitzGameState* GameState = Cast<ABinaryBlitzGameState>(GetWorld()->GetGameState()))
	{
		GameState->OnGameStateChanged.AddDynamic(this, &ABinaryBlitzUnitManager::OnGameStateChanged);
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

	if (const UBinaryBlitzConfig* Config = GetDefault<UBinaryBlitzConfig>())
	{
		UpdateInterval = Config->MovementUpdateInterval;
		MoveToMultiplier = Config->MoveToMultiplier;
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

	if (!bRun)
		return;

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

const AUnitBase* ABinaryBlitzUnitManager::GetBase(EFaction Faction) const
{
	return (Faction == EFaction::Good) ? GoodBase : EvilBase;
}

const TArray<AUnitBase*>& ABinaryBlitzUnitManager::GetActiveUnits(EFaction Faction) const
{
	return (Faction == EFaction::Good) ? GoodUnits : EvilUnits;
}

AUnitBase* ABinaryBlitzUnitManager::FindClosestEnemy(AUnitBase* Unit)
{
	if (!IsValid(Unit) || Unit->GetDefaultStats().ATK <= 0.0f)
		return nullptr;

	const TArray<AUnitBase*>& Opponents = (Unit->GetFaction() == EFaction::Good) ? EvilUnits : GoodUnits;
	AUnitBase* BestTarget = (Unit->GetFaction() == EFaction::Good) ? EvilBase : GoodBase;;
	float BestDistSq = FVector::DistSquared2D(Unit->GetActorLocation(), BestTarget->GetActorLocation());

	for (AUnitBase* Candidate : Opponents)
	{
		if (!IsValid(Candidate) || !Candidate->IsAlive())
		{
			continue;
		}

		const float DistSq = FVector::DistSquared2D(Unit->GetActorLocation(), Candidate->GetActorLocation());

		if (DistSq > FMath::Square(Unit->GetDefaultStats().PerceptionRange))
		{
			continue;
		}

		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestTarget = Candidate;
		}
	}

	return BestTarget;
}

void ABinaryBlitzUnitManager::UpdateInternal()
{
	auto UpdateSide = [this](AUnitBase* Base, const TArray<AUnitBase*>& Units)
		{
			AUnitBase* Target = FindClosestEnemy(Base);
			Base->SetTarget(Target);

			for (AUnitBase* Unit : Units)
			{
				if (!IsValid(Unit) || !Unit->ShouldUpdateTarget())
				{
					continue;
				}

				Target = FindClosestEnemy(Unit);
				Unit->SetTarget(Target);

				if (AUnitMovementController* UnitMovementController = Cast<AUnitMovementController>(Unit->GetController()))
				{
					const float DistSq = FVector::DistSquared2D(Unit->GetActorLocation(), Target->GetActorLocation());
					const float AttackRange = Unit->GetDefaultStats().Range;

					if (DistSq <= FMath::Square(AttackRange))
					{
						UnitMovementController->StopUnitMovement();
						UnitMovementController->SetCombatFocus(Target);
					}
					else
					{
						UnitMovementController->ClearCombatFocus();
						UnitMovementController->MoveToEnemy(Target, AttackRange * MoveToMultiplier);
					}
				}
			}
		};

	UpdateSide(GoodBase, GoodUnits);
	UpdateSide(EvilBase, EvilUnits);
}

void ABinaryBlitzUnitManager::OnGameStateChanged(EGameState State)
{
	if (State == EGameState::InProgress)
	{
		bRun = true;
	}
	else if (State == EGameState::Over)
	{
		bRun = false;

		for (AUnitBase* Unit : GoodUnits)
			Unit->Kill();
		for (AUnitBase* Unit : EvilUnits)
			Unit->Kill();
	}
}
