// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
class UProgressBar;

UCLASS( Abstract )
class SUN_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetOwnerActor(AActor* InActor) { OwnerActor = InActor;  }

protected:

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	TWeakObjectPtr<AActor> OwnerActor;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
};
