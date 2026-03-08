// Fill out your copyright notice in the Description page of Project Settings.

#include "BinaryBlitzUnitPool.h"
/* Unit */
#include "../Unit/UnitStats.h"
#include "../Unit/UnitBase.h"
/* Other */
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "../BinaryBlitzGameInstance.h"
#include "../BinaryBlitz.h"

ABinaryBlitzUnitPool* ABinaryBlitzUnitPool::Instance = nullptr;

ABinaryBlitzUnitPool::ABinaryBlitzUnitPool()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABinaryBlitzUnitPool::BeginPlay()
{
	Super::BeginPlay();

	if (!Instance)
	{
		Instance = this;
	}
	else
	{
		UE_LOG(LogBinaryBlitz, Warning, TEXT("Creating duplicate unit pool actor."))
	}

	PreWarm();
}

void ABinaryBlitzUnitPool::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Instance = nullptr;

	ClearPool();

	Super::EndPlay(EndPlayReason);
}

AActor* ABinaryBlitzUnitPool::AquireActor(EFaction Faction, EUnitType Type, const FVector& Position)
{
	if (!UnitPool.Contains(Faction))
		UnitPool.Add(Faction, { });

	if (!UnitPool[Faction].Contains(Type))
		UnitPool[Faction].Add(Type, { });

	for (FUnitPoolEntry& PoolEntry : UnitPool[Faction][Type])
	{
		if (PoolEntry.bInUse)
			continue;

		PoolEntry.bInUse = true;
		ActivateActor(PoolEntry.Unit, Position, Faction == EFaction::Good ? GoodSpawnRotation : EvilSpawnRotation);
		return PoolEntry.Unit;
	}

	AActor* NewActor = SpawnNewPooled(Faction, Type);
	if (!NewActor)
		return nullptr;

	ActivateActor(NewActor, Position, Faction == EFaction::Good ? GoodSpawnRotation : EvilSpawnRotation);
	UnitPool[Faction][Type].Add(FUnitPoolEntry(NewActor, true));
	return NewActor;
}

void ABinaryBlitzUnitPool::ReleaseActor(AActor* Actor)
{
	if (!Actor)
		return;

	for (auto& FactionMapPair : UnitPool)
	{
		for (auto& TypeMapPair : FactionMapPair.Value)
		{
			for (FUnitPoolEntry& PoolEntry : TypeMapPair.Value)
			{
				if (PoolEntry.Unit != Actor)
					continue;

				PoolEntry.bInUse = false;
				DeactivateActor(Actor);
				return;
			}
		}
	}
}

void ABinaryBlitzUnitPool::PreWarm()
{
	if (bWarmed)
		return;

	UBinaryBlitzGameInstance* GameInstance = Cast<UBinaryBlitzGameInstance>(GetGameInstance());
	if (!GameInstance || !GameInstance->GetUnitDataTable())
		return;

	for (const auto& TableRow : GameInstance->GetUnitDataTable()->GetRowMap())
	{
		if (const FUnitTableRow* TempUnitTableRow = reinterpret_cast<const FUnitTableRow*>(TableRow.Value))
		{
			if (!UnitRowName.Contains(TempUnitTableRow->Faction))
				UnitRowName.Add(TempUnitTableRow->Faction, { });

			if (!UnitRowName[TempUnitTableRow->Faction].Contains(TempUnitTableRow->Type))
				UnitRowName[TempUnitTableRow->Faction].Add(TempUnitTableRow->Type, { });


			UnitRowName[TempUnitTableRow->Faction][TempUnitTableRow->Type] = TableRow.Key;

			for (int i = 0; i < TempUnitTableRow->InitNum; i++)
			{
				if (!UnitPool.Contains(TempUnitTableRow->Faction))
					UnitPool.Add(TempUnitTableRow->Faction, { });

				if (!UnitPool[TempUnitTableRow->Faction].Contains(TempUnitTableRow->Type))
					UnitPool[TempUnitTableRow->Faction].Add(TempUnitTableRow->Type, { });

				AActor* NewActor = SpawnNewPooled(TempUnitTableRow->Faction, TempUnitTableRow->Type);
				if (!NewActor)
					continue;
				DeactivateActor(NewActor);
				UnitPool[TempUnitTableRow->Faction][TempUnitTableRow->Type].Add(FUnitPoolEntry(NewActor, false));
			}
		}
	}

	bWarmed = true;
}

void ABinaryBlitzUnitPool::ClearPool()
{
	for (auto& FactionMapPair : UnitPool)
	{
		for (auto& TypeMapPair : FactionMapPair.Value)
		{
			for (FUnitPoolEntry& PoolEntry : TypeMapPair.Value)
			{
				if (!IsValid(PoolEntry.Unit))
					continue;

				PoolEntry.Unit->Destroy();
			}
			TypeMapPair.Value.Empty();
		}
	}
}

AActor* ABinaryBlitzUnitPool::SpawnNewPooled(EFaction Faction, EUnitType Type)
{
	const UDataTable* UnitTable = nullptr;
	if (UBinaryBlitzGameInstance* GameInstance = Cast<UBinaryBlitzGameInstance>(GetGameInstance()))
		UnitTable = GameInstance->GetUnitDataTable();

	if (!IsValid(UnitTable))
	{
		UE_LOG(LogBinaryBlitz, Error, TEXT("Unit pool missing unit table."))
		return nullptr;
	}

#if WITH_EDITOR
	if (bWarmed)
	{
		UE_LOG(LogBinaryBlitz, Warning, TEXT("Spawning new pooled unit."))
	}
#endif

	if (!UnitRowName.Contains(Faction))
		UnitRowName.Add(Faction, { });

	if (!UnitRowName[Faction].Contains(Type))
		UnitRowName[Faction].Add(Type, { });

	FName RowName = UnitRowName[Faction][Type];
	const FUnitTableRow* UnitTableRow = UnitTable->FindRow<FUnitTableRow>(UnitRowName[Faction][Type], nullptr);
	if (!UnitTableRow)
	{
		UE_LOG(LogBinaryBlitz, Warning, TEXT("Unit table row name not cached."))

		for (const auto& TableRow : UnitTable->GetRowMap())
		{
			if (const FUnitTableRow* TempUnitTableRow = reinterpret_cast<const FUnitTableRow*>(TableRow.Value))
			{
				if (TempUnitTableRow->Faction == Faction && TempUnitTableRow->Type == Type)
				{
					RowName = TableRow.Key;
					UnitRowName[Faction][Type] = TableRow.Key;
					UnitTableRow = TempUnitTableRow;
					break;
				}
			}
		}
	}

	if (!UnitTableRow)
	{
		UE_LOG(LogBinaryBlitz, Error, TEXT("Unit not found in unit table."))
		return nullptr;
	}
	
	UClass* UnitClass = UnitTableRow->Asset.LoadSynchronous();

	if (!UnitClass)
	{
		UE_LOG(LogBinaryBlitz, Error, TEXT("Unit does not have class set in unit table."))
		return nullptr;
	}

	AActor* Actor = GetWorld()->SpawnActor(UnitClass, &InactivateLocation, &FRotator::ZeroRotator);
	if (IsValid(Actor))
	{
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
		Actor->SetActorTickEnabled(false);
		if (AUnitBase* UnitBase = Cast<AUnitBase>(Actor))
		{
			UnitBase->InitStats(Faction, UnitTable, RowName);
		}
	}

	return Actor;
}

void ABinaryBlitzUnitPool::ActivateActor(AActor* Actor, const FVector& Position, const FRotator& Rotation)
{
	FVector SpawnPosition = Position;
		
	AUnitBase* UnitBase = Cast<AUnitBase>(Actor);
	if (IsValid(UnitBase))
	{
		UCapsuleComponent* Capsule = UnitBase->GetCapsuleComponent();
		const float CapsuleRadius = Capsule->GetScaledCapsuleRadius() + 20.0f;
		const float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight() + 10.0f;

		FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FindFreeLocationForPooledCharacter), false);
		QueryParams.AddIgnoredActor(UnitBase);

		auto IsSpotFree = [&](const FVector& TestLocation) 	{
				return !GetWorld()->OverlapAnyTestByChannel(TestLocation, FQuat::Identity, ECC_Pawn, CapsuleShape, QueryParams);
			};

		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(Position, NavLocation, FVector(100.0f)))
		{
			if (IsSpotFree(NavLocation.Location)) // First try the exact desired position projected to nav
			{
				SpawnPosition = Position;
			}
			else // Then try nearby reachable points
			{
				UE_LOG(LogBinaryBlitz, Log, TEXT("Trying to spawn at random point on nav mesh."))

				for (int32 i = 0; i < 10; ++i)
				{
					FNavLocation Candidate;
					if (!NavSys->GetRandomReachablePointInRadius(Position, 10000.0f, Candidate))
						continue;

					if (IsSpotFree(Candidate.Location))
					{
						SpawnPosition = Candidate.Location;
						break;
					}
				}
			}
		}
	}

	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);
	Actor->SetActorTickEnabled(true);
	Actor->SetActorLocation(SpawnPosition + SpawnOffset, true);
	Actor->SetActorRotation(Rotation);
	if (IsValid(UnitBase))
	{
		UnitBase->OnSpawned();
	}
}

void ABinaryBlitzUnitPool::DeactivateActor(AActor* Actor)
{
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);
	Actor->SetActorLocation(InactivateLocation);
}
