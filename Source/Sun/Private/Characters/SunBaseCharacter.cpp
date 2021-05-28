// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SunBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Sun/Sun.h"
#include "TimerManager.h"
#include "Components/SHealthComponent.h"
#include "Components/WidgetComponent.h"
#include "HealthBar.h"


int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
ASunBaseCharacter::ASunBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	BaseTurnRate = 45.0f;
	BaseLookUpAtRate = 45.0f;

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20.0f;

	this->bAlwaysRelevant = true;

	MuzzleSocketName = "Muzzle";
	BaseDamage = 20.0f;
	RateOfFire = 600;

	bFiring = false;
}

void ASunBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;

	HealthComp->OnHealthChanged.AddDynamic(this, &ASunBaseCharacter::OnHealthChanged);
	UHealthBar* HealthBar = Cast<UHealthBar>(HealthWidgetComp->GetUserWidgetObject());
	HealthBar->SetOwnerActor(this);

	TimeBetweenShots = 60 / RateOfFire;
}

void ASunBaseCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);

}
void ASunBaseCharacter::MoveForward(float value)
{
	if ((Controller) && (value != 0.0f)) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}

void ASunBaseCharacter::MoveRight(float value)
{
	if ((Controller) && (value != 0.0f)) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);
	}
}


void ASunBaseCharacter::TurnAtRate(float value)
{
	AddControllerYawInput(value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASunBaseCharacter::Fire()
{
	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector ShotDirection = EyeRotation.Vector();
	FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

	FCollisionQueryParams QueryParams;
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

		UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, GetInstigatorController(), this, WeaponDamageType);

		UParticleSystem* SelectedEffect = nullptr;

		//UE_LOG(LogTemp, Warning, TEXT("Surface %s"), SurfaceType)
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

	if (DebugWeaponDrawing > 0)
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

	PlayFireEffects();

	LastTimeFired = GetWorld()->TimeSeconds;
}

void ASunBaseCharacter::StartFire()
{
	float FirstDelay = FMath::Max(LastTimeFired + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASunBaseCharacter::Fire, TimeBetweenShots, true, FirstDelay);

	bFiring = true;
}

void ASunBaseCharacter::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);

	bFiring = false;
}
void ASunBaseCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASunBaseCharacter::EndZoom()
{
	bWantsToZoom = false;
}

// Called to bind functionality to input
void ASunBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASunBaseCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASunBaseCharacter::StopFire);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASunBaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASunBaseCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASunBaseCharacter::TurnAtRate);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASunBaseCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASunBaseCharacter::EndZoom);
}

FVector ASunBaseCharacter::GetPawnViewLocation() const {
	if (CameraComp) {
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ASunBaseCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, 
	class AController* InstigatedBy, AActor* DamageCauser) {
	if (Health <= 0.0f && !bDied) {
		//Die :(
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);

	}
}

void ASunBaseCharacter::PlayFireEffects()
{
	if (MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, GetMesh(), MuzzleSocketName);

	if (TracerEffect)
		UGameplayStatics::SpawnEmitterAttached(TracerEffect, GetMesh(), MuzzleSocketName);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC) {
		PC->ClientStartCameraShake(FireCamShake);
	}
}
