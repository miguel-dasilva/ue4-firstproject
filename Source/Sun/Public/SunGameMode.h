// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SunGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SUN_API ASunGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHandle_NextWaveStart;
	//Bots to spawn in single wave
protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	void StartWave();

	void EndGameWin();

	void EndGameLose();

	void PrepareForNextWave();

	void CheckWaveState();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	bool GameWon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gamemode")
	int32 NrOfBotsToSpawn;

public:

	ASunGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};

