// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitBase.h"
#include "UnitMovementController.h"
#include "HealthBarWidget.h"
/* Components */
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
/* Managers */
#include "../System/BinaryBlitzUnitPool.h"
#include "../System/BinaryBlitzUnitManager.h"
#include "../BinaryBlitzGameState.h"
/* Other */
#include "Kismet/GameplayStatics.h"
#include "../BinaryBlitzConfig.h"
#include "../BinaryBlitz.h"

float AUnitBase::CoinDropMultiplier = -1.0f;
FUnitStats AUnitBase::FallbackStats = FUnitStats();
FEnemyAIFactors AUnitBase::FallbackAIFactors = FEnemyAIFactors();

AUnitBase::AUnitBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarComponent->SetupAttachment(GetRootComponent());
	HealthBarComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	HealthBarComponent->SetWidgetSpace(EWidgetSpace::World);
	HealthBarComponent->SetDrawSize(FVector2D(60.0f, 8.0f));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AUnitMovementController::StaticClass();

	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 150.f;

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
}
UE_DISABLE_OPTIMIZATION
void AUnitBase::BeginPlay()
{
	Super::BeginPlay();

	HP = GetDefaultStats().MaxHP;

	if (const UBinaryBlitzConfig* Config = GetDefault<UBinaryBlitzConfig>())
	{
		if (const FUnitTableRow* TableRow = TableRowHandle.GetRow<FUnitTableRow>(nullptr))
		{
			PassiveIncome = TableRow->Type == EUnitType::Base ?
				(Faction == EFaction::Good ? Config->GoodPassiveIncome : Config->EvilPassiveIncome) : 0;
		}
	}
	if (PassiveIncome > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(PassiveIncomeHandle, this, &AUnitBase::OnPassiveIncome, 1.0f, true);
	}
}

void AUnitBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (UnitState == EUnitState::Dead)
	{
		return;
	}

	UpdateHealthBarRotation();

	UpdateCombat(DeltaSeconds);
}

void AUnitBase::InitStats(const EFaction InFaction, const UDataTable* Table, const FName& RowName)
{
	Faction = InFaction;
	TableRowHandle.DataTable = Table;
	TableRowHandle.RowName = RowName;

	UCharacterMovementComponent* CharacterMovementComponent = GetComponentByClass<UCharacterMovementComponent>();
	if (IsValid(CharacterMovementComponent))
	{
		CharacterMovementComponent->MaxWalkSpeed = GetDefaultStats().Speed;

		CharacterMovementComponent->StopMovementImmediately();
		CharacterMovementComponent->Velocity = FVector::ZeroVector;
		CharacterMovementComponent->DisableMovement();
	}

	if (const UBinaryBlitzConfig* Config = GetDefault<UBinaryBlitzConfig>())
	{
		if (CoinDropMultiplier < 0.0f)
			CoinDropMultiplier = Config->CoinDropMultiplier;
	}
}

void AUnitBase::OnSpawned()
{
	HP = GetDefaultStats().MaxHP;
	UnitState = EUnitState::Idle;
	ABinaryBlitzUnitManager::GetInstance()->RegisterUnit(this);

	UCharacterMovementComponent* CharacterMovementComponent = GetComponentByClass<UCharacterMovementComponent>();
	if (IsValid(CharacterMovementComponent))
	{
		CharacterMovementComponent->StopMovementImmediately();
		CharacterMovementComponent->Velocity = FVector::ZeroVector;
		CharacterMovementComponent->SetMovementMode(MOVE_Walking);
	}

	if (HealthBarComponent && HealthBarComponent->GetWidget())
	{
		if (UHealthBarWidget* HealthBarWidget = Cast<UHealthBarWidget>(HealthBarComponent->GetWidget()))
		{
			HealthBarWidget->SetHealthPercent(1.0f);
		}
	}

	if (PassiveIncome > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(PassiveIncomeHandle, this, &AUnitBase::OnPassiveIncome, 1.0f, true);
	}
}

void AUnitBase::SetTarget(AUnitBase* InTarget)
{
	CurrentTarget = InTarget;
}

void AUnitBase::ReceiveDamage(float Amount)
{
	if (!IsAlive())
		return;

	HP -= Amount;

	if (HealthBarComponent && HealthBarComponent->GetWidget())
	{
		if (UHealthBarWidget* HealthBarWidget = Cast<UHealthBarWidget>(HealthBarComponent->GetWidget()))
		{
			HealthBarWidget->SetHealthPercent(GetHealthPercent());
		}
	}

	if (HP <= 0.f)
	{
		HP = 0.f;
		UnitState = EUnitState::Dead;
		OnDeath();
	}
}

bool AUnitBase::IsAlive() const
{
	return UnitState != EUnitState::Dead && HP > 0.0f;
}

bool AUnitBase::ShouldUpdateTarget() const
{
	return IsAlive() && UnitState != EUnitState::Attacking;
}

const FUnitStats& AUnitBase::GetDefaultStats() const
{
	if (TableRowHandle.IsNull())
		return FallbackStats;

	const FUnitTableRow* TableRow = TableRowHandle.GetRow<FUnitTableRow>(nullptr);
	return TableRow ? TableRow->Stats : FallbackStats;
}

const FEnemyAIFactors& AUnitBase::GetEnemyAIFactors() const
{
	if (TableRowHandle.IsNull())
		return FallbackAIFactors;

	const FUnitTableRow* TableRow = TableRowHandle.GetRow<FUnitTableRow>(nullptr);
	return TableRow ? TableRow->AIFactors : FallbackAIFactors;
}

float AUnitBase::GetHealthPercent() const
{
	return FMath::Clamp(HP / GetDefaultStats().MaxHP, 0.0f, 1.0f);
}

void AUnitBase::UpdateCombat(float DeltaSeconds)
{
	CooldownTimer -= DeltaSeconds;

	if (IsValid(CurrentTarget) && IsTargetInRange())
	{
		UnitState = EUnitState::Attacking;
		TryAttack();
	}
	else
	{
		UnitState = AIControllerClass != nullptr ? EUnitState::Moving : EUnitState::Idle;
	}
}

void AUnitBase::UpdateHealthBarRotation()
{
	if (!IsValid(HealthBarComponent))
		return;

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager)
		return;

	const FVector WidgetLocation = HealthBarComponent->GetComponentLocation();
	const FVector CameraLocation = CameraManager->GetCameraLocation();

	FRotator LookAtRotation = (CameraLocation - WidgetLocation).Rotation();

	LookAtRotation.Pitch = 0.f;
	LookAtRotation.Roll = 0.f;

	HealthBarComponent->SetWorldRotation(LookAtRotation);
}

bool AUnitBase::IsTargetInRange() const
{
	if (!IsValid(CurrentTarget))
		return false;

	// Ignore Z axis.
	const float DeltaX = GetActorLocation().X - CurrentTarget->GetActorLocation().X;
	const float DeltaY = GetActorLocation().Y - CurrentTarget->GetActorLocation().Y;
	const float DistSq = DeltaX * DeltaX + DeltaY * DeltaY;
	return DistSq <= FMath::Square(GetDefaultStats().Range);
}

void AUnitBase::TryAttack()
{
	if (CooldownTimer > 0.f || !IsValid(CurrentTarget))
		return;

	if (CurrentTarget->IsAlive())
	{
		CurrentTarget->ReceiveDamage(GetDefaultStats().ATK);
		CooldownTimer = GetDefaultStats().Cooldown;

		OnAttack_BP();
	}
}

void AUnitBase::OnDeath()
{
	OnDeath_BP();

	if (ABinaryBlitzGameState* GameState = Cast<ABinaryBlitzGameState>(GetWorld()->GetGameState()))
	{
		GameState->CollectMoney(Faction == EFaction::Evil ? EFaction::Good : EFaction::Evil,
			GetDefaultStats().Cost * FMath::Max(0.0f, CoinDropMultiplier));
	}
	
	GetWorld()->GetTimerManager().ClearTimer(PassiveIncomeHandle);
	PassiveIncomeHandle.Invalidate();

	UCharacterMovementComponent* CharacterMovementComponent = GetComponentByClass<UCharacterMovementComponent>();
	if (IsValid(CharacterMovementComponent))
	{
		CharacterMovementComponent->StopMovementImmediately();
		CharacterMovementComponent->Velocity = FVector::ZeroVector;
		CharacterMovementComponent->DisableMovement();
	}

	ABinaryBlitzUnitPool::GetInstance()->ReleaseActor(this);
	ABinaryBlitzUnitManager::GetInstance()->UnregisterUnit(this);
}

void AUnitBase::OnPassiveIncome()
{
	if (UnitState == EUnitState::Dead)
		return;

	if (ABinaryBlitzGameState* GameState = Cast<ABinaryBlitzGameState>(GetWorld()->GetGameState()))
		GameState->CollectMoney(Faction, PassiveIncome);
}

void AUnitBase::OnAttack_BP_Implementation()
{
	UE_LOG(LogBinaryBlitz, Log, TEXT("%s attack %s."), *GetName(), *CurrentTarget->GetName())
}

void AUnitBase::OnDeath_BP_Implementation()
{
	UE_LOG(LogBinaryBlitz, Log, TEXT("%s destroyed."), *GetName())
}
UE_ENABLE_OPTIMIZATION