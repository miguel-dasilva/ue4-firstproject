// Fill out your copyright notice in the Description page of Project Settings.


#include "Waypoint.h"
#include "Characters/SunAIMinion.h"
// Sets default values
AWaypoint::AWaypoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(Root);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComponent->SetupAttachment(GetRootComponent());
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWaypoint::OnPlayerEnter);
}

// Called when the game starts or when spawned
void AWaypoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWaypoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWaypoint::OnPlayerEnter(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, 
								bool bFromSweep, const FHitResult& SweepResult)
{
	ASunAIMinion* Minion = nullptr;
	if (OtherActor != nullptr) {
		Minion = Cast<ASunAIMinion>(OtherActor);
		if (Minion != nullptr) {
			Minion->NextWaypoint = NextWaypoint;
		}
	}
}

