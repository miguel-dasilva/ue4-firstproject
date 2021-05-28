// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SunAIMinion.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SHealthComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Waypoint.h"
#include "Sun/Sun.h"
#include "Components/WidgetComponent.h"
#include "HealthBar.h"
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Characters/SunBaseCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
ASunAIMinion::ASunAIMinion()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	bAttacking = false;
	BaseDamage = 1.0f;
}

// Called when the game starts or when spawned
void ASunAIMinion::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComp->OnHealthChanged.AddDynamic(this, &ASunAIMinion::OnHealthChanged);
	UHealthBar* HealthBar = Cast<UHealthBar>(HealthWidgetComp->GetUserWidgetObject());
	HealthBar->SetOwnerActor(this);
	NextPathPoint = GetNextPathPoint();

}

// Called every frame
void ASunAIMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const USkeletalMeshSocket* SwordBase = GetMesh()->GetSocketByName(FName("FX_Trail_01_L"));
	const USkeletalMeshSocket* SwordTip = GetMesh()->GetSocketByName(FName("FX_Trail_02_L"));
	if (SwordBase) {
		FVector EyeLocation;
		FRotator EyeRotation;
		GetActorEyesViewPoint(EyeLocation, EyeRotation);
		FVector HitDirection = EyeRotation.Vector();
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		FCollisionObjectQueryParams ObjectQueryParams;

		CollisionParams.AddIgnoredActor(this);
		CollisionParams.bTraceComplex = true;
		CollisionParams.bReturnPhysicalMaterial = true;
		//DrawDebugLine(GetWorld(), SwordBase->GetSocketLocation(GetMesh()), SwordTip->GetSocketLocation(GetMesh()), FColor(255, 0, 0), true);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, SwordBase->GetSocketLocation(GetMesh()), SwordTip->GetSocketLocation(GetMesh()), COLLISION_SWORD, CollisionParams)) {
			//Process Hit
			AActor* HitActor = HitResult.GetActor();

			if (HitActor) {
				float ActualDamage = BaseDamage;

				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, HitDirection, HitResult, GetInstigatorController(), this, WeaponDamageType);

				//UE_LOG(LogTemp, Warning, TEXT("Surface %s"), SurfaceType)

				if (DefaultImpactEffect)
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultImpactEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}
		}
	}
}

void ASunAIMinion::Attack() {
	if (!bAttacking) {
			bAttacking = true;
	}
}

void ASunAIMinion::StopAttack() {
	bAttacking = false;
}

void ASunAIMinion::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser) {
	if (Health <= 0.0f && !bDied) {
		//Die :(
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetLifeSpan(10.0f);

	}
}

FVector ASunAIMinion::GetNextPathPoint()
{
	//Only works in singleplayer i guess
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath && NavPath->PathPoints.Num() > 1) {
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

bool ASunAIMinion::GetAttacking() const {
	return bAttacking;
}