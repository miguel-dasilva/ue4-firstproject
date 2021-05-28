// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Sun/Sun.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
ASWeapon::ASWeapon()
{

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";

	BaseDamage = 20.0f;
}

void ASWeapon::Fire()
{
	// Trace the world, from pawn eyes to crosshair location

	AActor* MyOwner = GetOwner();
	if (MyOwner) {
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams)) {
			//Process hit
			AActor* HitActor = Hit.GetActor();
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
				ActualDamage *= 4.0f;

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			UParticleSystem* SelectedEffect = nullptr;

			UE_LOG(LogTemp, Warning, TEXT("Surface %s"), SurfaceType)
			switch (SurfaceType) {
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}

			if (SelectedEffect)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				
		}
		PlayFireEffects();
	}
	
	
}

void ASWeapon::PlayFireEffects() 
{

	if (MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);

	if (TracerEffect)
		UGameplayStatics::SpawnEmitterAttached(TracerEffect, MeshComp, MuzzleSocketName);

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner) {
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC) {
			PC->ClientStartCameraShake(FireCamShake);
		}
	}
}

