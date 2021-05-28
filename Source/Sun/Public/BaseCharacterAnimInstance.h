// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "BaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS(transient, Blueprintable, hideCategories=AnimInstance, BlueprintType)
class UBaseCharacterAnimInstance: public UAnimInstance
{
	GENERATED_BODY()
public:
	UBaseCharacterAnimInstance();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Generic")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Generic")
		bool IsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
		bool IsFiring;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};