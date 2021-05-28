
// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAnimInstance.h"
#include "Characters/SunBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBaseCharacterAnimInstance::UBaseCharacterAnimInstance()
{
	Speed = 0.0;
	IsInAir = false;
	IsFiring = false;
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);
	AActor* OwningActor = GetOwningActor();

	if (OwningActor != nullptr) {
		Speed = OwningActor->GetVelocity().Size();
		ASunBaseCharacter* OwningCharacter = Cast<ASunBaseCharacter>(OwningActor);
		if (OwningCharacter != nullptr) {
			IsInAir = OwningCharacter->GetCharacterMovement()->IsFalling();
		}
	}
}