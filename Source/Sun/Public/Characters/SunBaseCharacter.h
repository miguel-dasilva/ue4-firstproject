// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystem.h"
#include "SunBaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class ASWeapon;
class USHealthComponent;
class UDamageType;
class UParticleSystem;

UCLASS()
class SUN_API ASunBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASunBaseCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

protected:

	//Movement & Actions
	virtual void BeginPlay() override;
	void MoveForward(float value);
	void MoveRight(float value);
	void TurnAtRate(float value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpAtRate;

	bool bWantsToZoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	bool bFiring;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	float DefaultFOV;

	void BeginZoom();

	void EndZoom();

	//Health and health based events
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HealthWidgetComp;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category= "Player")
	bool bDied;

	//Weapon Functionalities
	void PlayFireEffects();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> WeaponDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UMatineeCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	void Fire();

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastTimeFired;

	/* RPM Bullets per min */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	float TimeBetweenShots;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();

};
