// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitBase.h"
#include "UnitMovementController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "../System/BinaryBlitzUnitPool.h"
#include "../System/BinaryBlitzUnitManager.h"
#include "../BinaryBlitz.h"

AUnitBase::AUnitBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AUnitMovementController::StaticClass();

	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 150.f;

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
}

void AUnitBase::BeginPlay()
{
	Super::BeginPlay();

	HP = DefaultStats.MaxHP;
}

void AUnitBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (UnitState == EUnitState::Dead)
	{
		return;
	}

	UpdateCombat(DeltaSeconds);
}

void AUnitBase::InitStats(const EFaction InFaction, const FUnitStats InDefaultStats)
{
	Faction = InFaction;
	DefaultStats = InDefaultStats;

	UCharacterMovementComponent* CharacterMovementComponent = GetComponentByClass<UCharacterMovementComponent>();
	if (IsValid(CharacterMovementComponent))
	{
		CharacterMovementComponent->MaxWalkSpeed = DefaultStats.Speed;

		CharacterMovementComponent->StopMovementImmediately();
		CharacterMovementComponent->Velocity = FVector::ZeroVector;
		CharacterMovementComponent->DisableMovement();
	}
}

void AUnitBase::OnSpawned()
{
	HP = DefaultStats.MaxHP;
	UnitState = EUnitState::Idle;
	ABinaryBlitzUnitManager::GetInstance()->RegisterUnit(this);

	UCharacterMovementComponent* CharacterMovementComponent = GetComponentByClass<UCharacterMovementComponent>();
	if (IsValid(CharacterMovementComponent))
	{
		CharacterMovementComponent->StopMovementImmediately();
		CharacterMovementComponent->Velocity = FVector::ZeroVector;
		CharacterMovementComponent->SetMovementMode(MOVE_Walking);
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

bool AUnitBase::IsTargetInRange() const
{
	if (!IsValid(CurrentTarget))
		return false;

	const float DistSq = FVector::DistSquared(GetActorLocation(), CurrentTarget->GetActorLocation());
	return DistSq <= FMath::Square(DefaultStats.Range);
}

void AUnitBase::TryAttack()
{
	if (CooldownTimer > 0.f || !IsValid(CurrentTarget))
		return;

	if (CurrentTarget->IsAlive())
	{
		UE_LOG(LogBinaryBlitz, Log, TEXT("%s attack %s."), *GetName(), *CurrentTarget->GetName())

		CurrentTarget->ReceiveDamage(DefaultStats.ATK);
		CooldownTimer = DefaultStats.Cooldown;
	}
}

void AUnitBase::OnDeath()
{
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
