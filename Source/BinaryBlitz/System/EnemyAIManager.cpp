// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIManager.h"
/* Actors */
#include "RegionTrigger.h"
#include "../Unit/UnitBase.h"
/* Managers */
#include "BinaryBlitzUnitManager.h"
#include "BinaryBlitzUnitPool.h"
#include "../BinaryBlitzGameState.h"
#include "../BinaryBlitzGameInstance.h"
/* Other */
#include "Kismet/GameplayStatics.h"
#include "Algo/SelectRandomWeighted.h"
#include "../BinaryBlitzConfig.h"
#include "../BinaryBlitz.h"

AEnemyAIManager* AEnemyAIManager::Instance = nullptr;

AEnemyAIManager::AEnemyAIManager()
{
	PrimaryActorTick.bCanEverTick = true;
}
UE_DISABLE_OPTIMIZATION
void AEnemyAIManager::BeginPlay()
{
	Super::BeginPlay();

	if (!Instance)
	{
		Instance = this;
	}
	else
	{
		UE_LOG(LogBinaryBlitz, Warning, TEXT("Creating duplicate enemy AI actor."))
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARegionTrigger::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (ARegionTrigger* RegionTrigger = Cast<ARegionTrigger>(Actor))
		{
			RegionTriggerIndex.Add(RegionTrigger->GetRegion(), RegionTriggers.Num());
			RegionTriggers.Add(RegionTrigger);
		}
	}

	auto InitScore = []() {
		TMap<EMapRegion, float> TempScore;
		for (EMapRegion Region : TEnumRange<EMapRegion>())
			TempScore.Add(Region, 0.0f);
		return TempScore;
		};

	RegionThreatScore = InitScore();
	RegionAttackScore = InitScore();

	if (const UBinaryBlitzConfig* Config = GetDefault<UBinaryBlitzConfig>())
	{
		DefendThreshold = Config->DefendThreshold;
		AttackThreshold = Config->AttackThreshold;
		BaseLowHealthPercent = Config->BaseLowHealthPercent;
		PriorityWeight = Config->PriorityWeight;
		OtherWeight = Config->OtherWeight;
		WaitWeight = Config->WaitWeight;
		UpdateInterval = Config->AIUpdateInterval;
	}

	UBinaryBlitzGameInstance* GameInstance = Cast<UBinaryBlitzGameInstance>(GetGameInstance());
	if (!GameInstance || !GameInstance->GetUnitDataTable())
		return;

	for (const auto& TableRow : GameInstance->GetUnitDataTable()->GetRowMap())
	{
		if (const FUnitTableRow* TempUnitTableRow = reinterpret_cast<const FUnitTableRow*>(TableRow.Value))
		{
			if (TempUnitTableRow->Faction == EFaction::Good || TempUnitTableRow->Type == EUnitType::Base)
				continue;

			int TempCost = static_cast<int>(TempUnitTableRow->Stats.Cost);
			UnitCostMap.Add(TempUnitTableRow->Type, TempCost);
		}
	}
}

void AEnemyAIManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Instance = nullptr;

	Super::EndPlay(EndPlayReason);
}

void AEnemyAIManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateTimer += DeltaSeconds;
	SpawnCooldownTimer -= DeltaSeconds;
	if (UpdateTimer >= UpdateInterval && SpawnCooldownTimer <= 0.0f)
	{
		UpdateTimer = 0.0f;
		UpdateInternal();
	}
}

void AEnemyAIManager::UpdateInternal()
{
	CalculateRegionScores();
	
	FSpawnAction SpawnAction = DetermineAction();

	if (SpawnAction.Action == ESpawnAction::Wait || SpawnAction.Region == EMapRegion::Unknown)
		return;

	EUnitType SpawnUnitType = DetermineUnitType();
	FSpawnResult SpawnResult = TrySpawnUnit(SpawnUnitType, RegionTriggers[RegionTriggerIndex[SpawnAction.Region]]->GetSpawnLocation());

	if (SpawnResult.Result == ESpawnResult::Spawned && SpawnResult.Unit)
	{
		SpawnResult.Unit->OnSpawned();
	}
}

void AEnemyAIManager::CalculateRegionScores()
{
	for (EMapRegion Region : TEnumRange<EMapRegion>())
	{
		RegionThreatScore[Region] = 0.0f;
		RegionAttackScore[Region] = 0.0f;
	}

	for (AUnitBase* Unit : ABinaryBlitzUnitManager::GetInstance()->GetActiveUnits(EFaction::Good))
	{
		const FEnemyAIFactors& Factor = Unit->GetEnemyAIFactors();
		for (const ARegionTrigger* RegionTrigger : RegionTriggers)
		{
			if (RegionTrigger->ContainsActor(Unit))
			{
				RegionThreatScore[RegionTrigger->GetRegion()] += Factor.OffenceScore * Factor.Bias * Unit->GetHealthPercent();
				RegionAttackScore[RegionTrigger->GetRegion()] += Factor.DefenceScore * Factor.Bias * Unit->GetHealthPercent();
				break;
			}
		}
	}

	for (AUnitBase* Unit : ABinaryBlitzUnitManager::GetInstance()->GetActiveUnits(EFaction::Evil))
	{
		const FEnemyAIFactors& Factor = Unit->GetEnemyAIFactors();
		for (const ARegionTrigger* RegionTrigger : RegionTriggers)
		{
			if (RegionTrigger->ContainsActor(Unit))
			{
				RegionThreatScore[RegionTrigger->GetRegion()] += Factor.DefenceScore * Factor.Bias * Unit->GetHealthPercent();
				RegionAttackScore[RegionTrigger->GetRegion()] += Factor.OffenceScore * Factor.Bias * Unit->GetHealthPercent();
				break;
			}
		}
	}

#if WITH_EDITORONLY_DATA
	if (bDebugScores)
	{
		FString DebugText;
		for (EMapRegion Region : TEnumRange<EMapRegion>())
		{
			const UEnum* EnumPtr = StaticEnum<EMapRegion>();
			FString EnumName = EnumPtr ? EnumPtr->GetNameStringByValue((int64)Region) : TEXT("Invalid");
			DebugText += FString::Printf(TEXT("%-20s : %2.2f | %2.2f\n"), *EnumName, RegionThreatScore[Region], RegionAttackScore[Region]);
		}
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Green, DebugText);
		}
	}
#endif
}

AEnemyAIManager::FSpawnAction AEnemyAIManager::DetermineAction()
{
	struct FRegionSample
	{
		int Index;
		float Weight;

		FRegionSample() : Index(0), Weight(0.0f) { }
		FRegionSample(int InIndex, float InWeight) : Index(InIndex), Weight(InWeight) { }
	};

	static TArray<EMapRegion> GoodLaneRegions = { EMapRegion::GoodLeft, EMapRegion::GoodMid, EMapRegion::GoodRight };
	static TArray<EMapRegion> EvilLaneRegions = { EMapRegion::EvilLeft, EMapRegion::EvilMid, EMapRegion::EvilRight };

	const AUnitBase* Base = ABinaryBlitzUnitManager::GetInstance()->GetBase(EFaction::Evil);
	if (Base->GetHealthPercent() <= BaseLowHealthPercent || RegionThreatScore[EMapRegion::EvilBase] >= DefendThreshold)
	{
		return AEnemyAIManager::FSpawnAction(EMapRegion::EvilBase);
	}

	auto DetermineAction = [this](const TArray<EMapRegion>& Regions, bool bOffence) {
			float Priority = TNumericLimits<float>::Min();
			EMapRegion PriorityRegion = EMapRegion::EvilBase;

			for (EMapRegion Region : Regions)
			{
				const float TempPriority = (RegionThreatScore[Region] - RegionAttackScore[Region]) * (bOffence ? -1.0f : 1.0f);
				if (TempPriority > Priority)
				{
					Priority = TempPriority;
					PriorityRegion = Region;
				}
			}

			if (Priority >= (bOffence ? AttackThreshold : DefendThreshold))
				return AEnemyAIManager::FSpawnAction(PriorityRegion);

			TArray<FRegionSample> RegionSamples;
			RegionSamples.Add(FRegionSample(-1, WaitWeight));
			for (int i = 0; i < Regions.Num(); i++)
				RegionSamples.Add(FRegionSample(i, Regions[i] == PriorityRegion ? PriorityWeight : OtherWeight));

			if (const FRegionSample* SampleResult = Algo::SelectRandomWeightedBy(RegionSamples, [](const FRegionSample& Sample) { return Sample.Weight; }))
			{
				if (SampleResult->Index == -1)
					return AEnemyAIManager::FSpawnAction();
				else
					return AEnemyAIManager::FSpawnAction(Regions[SampleResult->Index]);
			}

			return AEnemyAIManager::FSpawnAction();
		};

	

	if (RegionThreatScore[EMapRegion::EvilLeft] == 0.0f &&
		RegionThreatScore[EMapRegion::EvilMid] == 0.0f &&
		RegionThreatScore[EMapRegion::EvilRight] == 0.0f)
	{
		return DetermineAction(GoodLaneRegions, true);
	}
	else
	{
		return DetermineAction(EvilLaneRegions, false);
	}
}

EUnitType AEnemyAIManager::DetermineUnitType()
{
	struct FUnitTypeSample
	{
		EUnitType Type;
		float Weight;

		FUnitTypeSample(EUnitType InType, float InWeight) : Type(InType), Weight(InWeight) { }
	};

	TArray<FUnitTypeSample> UnitTypeSamples;
	UnitTypeSamples.Add(FUnitTypeSample(EUnitType::Small, 10.0f));
	if (ABinaryBlitzGameState* GameState = Cast<ABinaryBlitzGameState>(GetWorld()->GetGameState()))
	{
		const int Money = GameState->GetMoney(EFaction::Evil);

		if (Money >= UnitCostMap[EUnitType::Large] * 2)
			UnitTypeSamples.Add(FUnitTypeSample(EUnitType::Large, 7.0f));
		else if (Money >= UnitCostMap[EUnitType::Large])
			UnitTypeSamples.Add(FUnitTypeSample(EUnitType::Large, 3.0f));
		else
			UnitTypeSamples.Add(FUnitTypeSample(EUnitType::Large, 1.0f));

		if (Money >= UnitCostMap[EUnitType::Medium] * 2)
			UnitTypeSamples.Add(FUnitTypeSample(EUnitType::Medium, 10.0f));
		else if (Money >= UnitCostMap[EUnitType::Medium])
			UnitTypeSamples.Add(FUnitTypeSample(EUnitType::Medium, 7.0f));
		else
			UnitTypeSamples.Add(FUnitTypeSample(EUnitType::Medium, 1.0f));
	}

	if (const FUnitTypeSample* SampleResult = Algo::SelectRandomWeightedBy(UnitTypeSamples, [](const FUnitTypeSample& Sample) { return Sample.Weight; }))
	{
		return SampleResult->Type;
	}
	return EUnitType::Small;
}

AEnemyAIManager::FSpawnResult AEnemyAIManager::TrySpawnUnit(EUnitType Type, const FVector& Position)
{
	if (!UnitCostMap.Contains(Type))
		return AEnemyAIManager::FSpawnResult();

	if (ABinaryBlitzGameState* GameState = Cast<ABinaryBlitzGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->PurchaseUnit(EFaction::Evil, UnitCostMap[Type]))
		{
			AUnitBase* Unit = Cast<AUnitBase>(ABinaryBlitzUnitPool::GetInstance()->AquireActor(EFaction::Evil, Type, Position));
			SpawnCooldownTimer = Unit->GetDefaultStats().SpawnCooldown;
			return AEnemyAIManager::FSpawnResult(Unit);
		}
	}
	return AEnemyAIManager::FSpawnResult();
}
UE_ENABLE_OPTIMIZATION