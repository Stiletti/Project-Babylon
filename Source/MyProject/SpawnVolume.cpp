// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Critter.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));

}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	FVector Extent = SpawnBox->GetScaledBoxExtent(); // gets the extent of the spawnbox
	FVector Origin = SpawnBox->GetComponentLocation(); // gets the origin of the spawnbox

	FVector SpawnPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent); // sets a random spawnpoint within the spawnbox

	return SpawnPoint;
}

void ASpawnVolume::SpawnOurPawn_Implementation(UClass* ToSpawn, const FVector& Location) // _Implementation will send some Parts to Blueprintsystem
{
	if (ToSpawn)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;

		if (World)
		{
			ACritter* CritterSpawned = World->SpawnActor<ACritter>(ToSpawn, Location, FRotator(0.0f), SpawnParams);
		}
	}
}

