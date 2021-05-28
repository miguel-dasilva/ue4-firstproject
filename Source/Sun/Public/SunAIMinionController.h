// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SunAIMinionController.generated.h"

/**
 * 
 */
UCLASS()
class SUN_API ASunAIMinionController : public AAIController
{
	GENERATED_BODY()
public:
	ASunAIMinionController();

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* PossessPawn) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual FRotator GetControlRotation() const override;

	UFUNCTION()
	void OnPawnDetected(const TArray<AActor*>&DetectedPawns);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float AISightRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float AISightAge;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float AILoseSightRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float AIFieldOfView;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsPlayerDetected;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	float DistanceToPlayer;

};
