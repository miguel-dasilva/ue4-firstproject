// Fill out your copyright notice in the Description page of Project Settings.


#include "SunAIMinionController.h"
#include "Characters/SunAIMinion.h"
#include "Waypoint.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/SunBaseCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ASunAIMinionController::ASunAIMinionController() {
	PrimaryActorTick.bCanEverTick = true;

	AISightRadius = 500.0f;
	AISightAge = 5.0f;
	AILoseSightRadius = AISightRadius + 50.0f;
	AIFieldOfView = 90.0f;
	bIsPlayerDetected = false;
	DistanceToPlayer = 100000.0f;

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp")));

	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	SightConfig->SetMaxAge(AISightAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ASunAIMinionController::OnPawnDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);

}

void ASunAIMinionController::BeginPlay() {
	Super::BeginPlay();

	if (GetPerceptionComponent() != nullptr)
		UE_LOG(LogTemp, Warning, TEXT("System set"));

	UE_LOG(LogTemp, Warning, TEXT("Problem"));
}

void ASunAIMinionController::OnPossess(APawn* PossessPawn) {
	Super::OnPossess(PossessPawn);
}

void ASunAIMinionController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ASunAIMinion* MeleeMinion = Cast<ASunAIMinion>(GetPawn());
	if (MeleeMinion) {
		if (bIsPlayerDetected == true ) {
			ASunBaseCharacter* Player = Cast<ASunBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			if (GetPawn()->GetDistanceTo(Player) < 400.0f) {
				MeleeMinion->Attack();
			}
			else {
				MoveToActor(Player, 5.0f);
				MeleeMinion->StopAttack();
			}
		} else {
			MoveToLocation(MeleeMinion->GetNextPathPoint(), 5.0f);
		}
	}
}

FRotator ASunAIMinionController::GetControlRotation() const
{
	if (GetPawn() == nullptr) {
		return FRotator(0.0f, 0.0f, 0.0f);
	}

	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);
}

void ASunAIMinionController::OnPawnDetected(const TArray<AActor*>& DetectedPawns)
{
	for (size_t i = 0; i < DetectedPawns.Num(); i++) {
		DistanceToPlayer = GetPawn()->GetDistanceTo(DetectedPawns[i]);

		UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), DistanceToPlayer);
	}

	bIsPlayerDetected = true;
}
