// Fill out your copyright notice in the Description page of Project Settings.


#include "Floater.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloater::AFloater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	RunningTime = 0.0f; 

	Amplitude = 1.0f;
	TimeStretch = 1.0f;
}

// Called when the game starts or when spawned
void AFloater::BeginPlay()
{
	Super::BeginPlay();

	FHitResult HitResult;
	
	float SpawnX = FMath::FRandRange(-500.0f, 500.0f);
	float SpawnY = FMath::FRandRange(-500.0f, 500.0f);
	float SpawnZ = FMath::FRandRange(0.0f, 500.0f);

	SpawnPoint.X = SpawnX;
	SpawnPoint.Y = SpawnY;
	SpawnPoint.Z = SpawnZ;

	SetActorLocation(SpawnPoint, false, &HitResult);
}

// Called every frame
void AFloater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation();

	NewLocation.Z += Amplitude * FMath::Sin(TimeStretch * RunningTime);
	NewLocation.X += Amplitude * FMath::Sin(TimeStretch * RunningTime);
	NewLocation.Y += Amplitude * FMath::Cos(TimeStretch * RunningTime); // + 20.0f; 
	// 1. Wert modeliert die gesamte Sinuskurve bzw dessen max Peak (1.0f = Default) auch Amplitude genannt
	// 2. Wert modeliert die Ausführungszeit (streckt quasi die Sinuskurve)
	// 3. Wert fügt der Sinuskurve einen selbstdefinierten Wert zum Peak hinzu (NUR ZUM OBEREN PEAK, Object steigt immer höher)

	RunningTime += DeltaTime;
	SetActorLocation(NewLocation);
}

