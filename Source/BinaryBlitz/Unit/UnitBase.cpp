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
#include "Navigation/PathFollowingComponent.h"
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

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void AUnitBase::BeginPlay()
{
	Super::BeginPlay();

	HP = GetDefaultStats().MaxHP;
}

void AUnitBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (UnitState == EUnitState::Dead)
	{
		return;
	}

	// Death plane.
	if (GetActorLocation().Z <= -1200.0f)
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		GetCharacterMovement()->DisableMovement();

		ABinaryBlitzUnitPool::GetInstance()->ReleaseActor(this);
		ABinaryBlitzUnitManager::GetInstance()->UnregisterUnit(this);
	}

	UpdateHealthBarRotation();

	UpdateCombat(DeltaSeconds);
}

void AUnitBase::InitStats(const EFaction InFaction, const UDataTable* Table, const FName& RowName)
{
	Faction = InFaction;
	TableRowHandle.DataTable = Table;
	TableRowHandle.RowName = RowName;

	GetCharacterMovement()->MaxWalkSpeed = GetDefaultStats().Speed;
	GetCharacterMovement()->RotationRate = FRotator(0.f, GetDefaultStats().RotateSpeed, 0.f);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	GetCharacterMovement()->DisableMovement();

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

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	if (HealthBarComponent && HealthBarComponent->GetWidget())
	{
		if (UHealthBarWidget* HealthBarWidget = Cast<UHealthBarWidget>(HealthBarComponent->GetWidget()))
		{
			HealthBarWidget->SetHealthPercent(1.0f);
		}
	}

	StartPassiveIncome();
}

void AUnitBase::SetTarget(AUnitBase* InTarget)
{
	CurrentTarget = InTarget;

	if (AUnitMovementController* UnitMovementController = Cast<AUnitMovementController>(GetController()))
	{
		if (IsValid(CurrentTarget))
			UnitMovementController->SetCombatFocus(CurrentTarget);
		else
			UnitMovementController->ClearCombatFocus();
	}
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

void AUnitBase::StartPassiveIncome()
{
	if (GetDefaultStats().PassiveIncome > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(PassiveIncomeHandle, this, &AUnitBase::OnPassiveIncome, 1.0f, true);
	}
}

void AUnitBase::Kill()
{
	UnitState = EUnitState::Dead;

	GetWorld()->GetTimerManager().ClearTimer(PassiveIncomeHandle);
	PassiveIncomeHandle.Invalidate();

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	GetCharacterMovement()->DisableMovement();
}

void AUnitBase::UpdateCombat(float DeltaSeconds)
{
	CooldownTimer -= DeltaSeconds;

	AUnitMovementController* UnitMovementController = Cast<AUnitMovementController>(GetController());

	if (IsValid(CurrentTarget) && IsTargetInRange())
	{
		if (IsValid(UnitMovementController))
		{
			//UnitMovementController->StopUnitMovement();
			UnitMovementController->SetCombatFocus(CurrentTarget);
		}

		if (IsFacingTarget())
		{
			UnitState = EUnitState::Attacking;
			TryAttack();
		}
		else
		{
			UnitState = EUnitState::TurningToAttack;
		}
	}
	else
	{
		if (IsValid(UnitMovementController))
		{
			UnitMovementController->ClearCombatFocus();
			UnitState = UnitMovementController->GetMoveStatus() == EPathFollowingStatus::Type::Moving ? EUnitState::Moving : EUnitState::Idle;
		}
		else
		{
			UnitState = EUnitState::Idle;
		}
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

	const float DistSq = FVector::DistSquared2D(GetActorLocation(), CurrentTarget->GetActorLocation());
	return DistSq <= FMath::Square(GetDefaultStats().Range);
}

bool AUnitBase::IsFacingTarget() const
{
	if (!IsValid(CurrentTarget))
		return false;

	const FVector Forward = GetActorForwardVector().GetSafeNormal2D();
	const FVector ToTarget = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();

	const float Dot = FVector::DotProduct(Forward, ToTarget);
	return Dot >= GetDefaultStats().FacingDot;
}

void AUnitBase::TryAttack()
{
	if (CooldownTimer > 0.f || !IsValid(CurrentTarget))
		return;

	if (CurrentTarget->IsAlive())
	{
		CurrentTarget->ReceiveDamage(GetDefaultStats().ATK);
		if (!CurrentTarget->IsAlive())
		{
			if (AUnitMovementController* UnitMovementController = Cast<AUnitMovementController>(GetController()))
			{
				UnitMovementController->ClearCombatFocus();
			}
		}
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

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	GetCharacterMovement()->DisableMovement();

	ABinaryBlitzUnitPool::GetInstance()->ReleaseActor(this);
	ABinaryBlitzUnitManager::GetInstance()->UnregisterUnit(this);
}

void AUnitBase::OnPassiveIncome()
{
	if (UnitState == EUnitState::Dead)
		return;

	if (ABinaryBlitzGameState* GameState = Cast<ABinaryBlitzGameState>(GetWorld()->GetGameState()))
		GameState->CollectMoney(Faction, GetDefaultStats().PassiveIncome);
}

void AUnitBase::OnAttack_BP_Implementation()
{
	UE_LOG(LogBinaryBlitz, Log, TEXT("%s attack %s."), *GetName(), *CurrentTarget->GetName())
}

void AUnitBase::OnDeath_BP_Implementation()
{
	UE_LOG(LogBinaryBlitz, Log, TEXT("%s destroyed."), *GetName())
}
