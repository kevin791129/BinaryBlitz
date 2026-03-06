// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnitStats.h"
#include "UnitBase.generated.h"

UENUM(BlueprintType)
enum class EUnitState : uint8
{
	Idle,
	Moving,
	Attacking,
	Dead
};

UCLASS(Abstract, HideDropdown)
class BINARYBLITZ_API AUnitBase : public ACharacter
{
	GENERATED_BODY()
	
public:	
	//~ Begin AActor Interface
	virtual void Tick(float DeltaSeconds) override;
	//~ End AActor Interface

	AUnitBase();

	void InitStats(const EFaction InFaction, const FUnitStats InDefaultStats);

	void OnSpawned();

	void SetTarget(AUnitBase* InTarget);

	void ReceiveDamage(float Amount);

	bool IsAlive() const;

	EFaction GetFaction() const { return Faction; }

	float GetAttackRange() const { return DefaultStats.Range; }

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	void UpdateCombat(float DeltaSeconds);

	bool IsTargetInRange() const;

	void TryAttack();

	void OnDeath();

protected:
	/** Hit point */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Stats, meta = (DisplayName = "HP"))
	float HP;
	/** Attack cooldown timer */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Stats, meta = (DisplayName = "Attack Cooldown"))
	float CooldownTimer;
	/** Faction */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats, meta = (DisplayName = "Faction"))
	EFaction Faction;
	/** Initial stats */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats, meta = (DisplayName = "Default Stats"))
	FUnitStats DefaultStats;

	/** Current unit state */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = State, meta = (DisplayName = "Current State"))
	EUnitState UnitState = EUnitState::Idle;
	/** Current attack target */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = State, meta = (DisplayName = "Current Target"))
	AUnitBase* CurrentTarget = nullptr;
};
