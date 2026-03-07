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

class UWidgetComponent;

UCLASS(Abstract, HideDropdown)
class BINARYBLITZ_API AUnitBase : public ACharacter
{
	GENERATED_BODY()
	
public:	
	//~ Begin AActor Interface
	virtual void Tick(float DeltaSeconds) override;
	//~ End AActor Interface

	AUnitBase();

	void InitStats(const EFaction InFaction, const UDataTable* Table, const FName& RowName);

	void OnSpawned();

	void SetTarget(AUnitBase* InTarget);

	void ReceiveDamage(float Amount);

	bool IsAlive() const;

	bool ShouldUpdateTarget() const;

	EUnitState GetState() const { return UnitState; }

	EFaction GetFaction() const { return Faction; }

	const FUnitStats& GetDefaultStats() const;

	const FEnemyAIFactors& GetEnemyAIFactors() const;

	float GetHealthPercent() const;

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	void UpdateCombat(float DeltaSeconds);

	void UpdateHealthBarRotation();

	bool IsTargetInRange() const;

	void TryAttack();

	void OnDeath();

	void OnPassiveIncome();

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "On Attack BP"))
	void OnAttack_BP();

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "On Death BP"))
	void OnDeath_BP();

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
	/** Data table row handle */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (DisplayName = "Table Row Handle"))
	FDataTableRowHandle TableRowHandle;

	/** Current unit state */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = State, meta = (DisplayName = "Current State"))
	EUnitState UnitState = EUnitState::Idle;
	/** Current attack target */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = State, meta = (DisplayName = "Current Target"))
	AUnitBase* CurrentTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* HealthBarComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats, meta = (DisplayName = "Income Generation"))
	float PassiveIncome;
	static float CoinDropMultiplier;

private:
	static FUnitStats FallbackStats;
	static FEnemyAIFactors FallbackAIFactors;

	FTimerHandle PassiveIncomeHandle;
};
