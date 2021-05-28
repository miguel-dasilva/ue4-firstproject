// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SHealthComponent.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100;

	TeamNum = 255;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner) {
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
	}

	Health = DefaultHealth;
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {
	if (Damage <= 0.0f) {
		return;
	}

	if (isFriendly(DamagedActor, DamageCauser))
		return;

	Health = FMath::Max(0.0f, Health - Damage);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

float USHealthComponent::GetHealth() const
{
	return Health;
}
float USHealthComponent::GetDefaultHealth() const
{
	return DefaultHealth;
}

bool USHealthComponent::isFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
		return true;

	USHealthComponent* HealthCompA = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* HealthCompB = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));
	
	if (HealthCompA == nullptr || HealthCompB == nullptr)
		return true;

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}
