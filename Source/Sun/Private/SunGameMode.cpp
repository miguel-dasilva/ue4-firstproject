// Fill out your copyright notice in the Description page of Project Settings.


#include "SunGameMode.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "SunGameState.h"
#include "Components/SHealthComponent.h"

ASunGameMode::ASunGameMode()
{
	GameStateClass = ASunGameState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
	GameWon = false;

}

void ASunGameMode::StartWave()
{
	NrOfBotsToSpawn = 5;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASunGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
}

void ASunGameMode::EndGameWin()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	UE_LOG(LogTemp, Log, TEXT("Player Won!"))
	GameWon = true;
}

void ASunGameMode::EndGameLose()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	UE_LOG(LogTemp, Log, TEXT("Player Lost!"))
}

void ASunGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASunGameMode::StartWave, 1.0f, false);
}

void ASunGameMode::StartPlay() {
	Super::StartPlay();

	PrepareForNextWave();
}
void ASunGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;

}

void ASunGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0 || bIsPreparingForWave) {
		return;
	}
	bool bIsAnyBotAlive = false;
	bool bIsPlayerAlive = false;
	for (TActorIterator<APawn> Itr(GetWorld()); Itr; ++Itr) {

		APawn* TestPawn = *Itr;
		if (TestPawn == nullptr)
			continue;

		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));

		if (TestPawn->IsPlayerControlled()) {
			UE_LOG(LogTemp, Warning, TEXT("PlayerControlled"));
			if (HealthComp && HealthComp->GetHealth() > 0.0f) {
				bIsPlayerAlive = true;
				continue;
			}
		}
		else {
			if (HealthComp && HealthComp->GetHealth() > 0.0f) {
				UE_LOG(LogTemp, Warning, TEXT("~Bot"));
				bIsAnyBotAlive = true;
				break;
			}
		}
	}

	if (!bIsAnyBotAlive)
		EndGameWin();

	if (!bIsPlayerAlive)
		EndGameLose();
}

void ASunGameMode::Tick(float DeltaSeconds) {

	Super::Tick(DeltaSeconds);

	CheckWaveState();
}