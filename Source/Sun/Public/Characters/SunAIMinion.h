// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SunAIMinion.generated.h"

class USHealthComponent;
class USkeletalMeshSocket;
class UDamageType;
class USphereComponent;

UCLASS()
class SUN_API ASunAIMinion : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASunAIMinion();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AWaypoint* NextWaypoint;

	UPROPERTY(BlueprintReadOnly, Category = "Minion")
	bool bDied;

	UPROPERTY(BlueprintReadWrite, Category = "Minion")
	bool bAttacking;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> WeaponDamageType;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	FTimerHandle AttackTimerHandle;

	void StopAttack();
	void Attack();

	FVector GetNextPathPoint();

	FVector NextPathPoint;

	bool GetAttacking() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HealthWidgetComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};