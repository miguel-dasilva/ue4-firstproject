// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthBar.h"
#include "Components/SHealthComponent.h"
#include "Components/ProgressBar.h"

void UHealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!OwnerActor.IsValid())
		return;

	USHealthComponent* HealthComp = Cast<USHealthComponent>(OwnerActor->GetComponentByClass(USHealthComponent::StaticClass()));
	if (HealthComp) {
		HealthBar->SetPercent(HealthComp->GetHealth() / HealthComp->GetDefaultHealth());
	}
}