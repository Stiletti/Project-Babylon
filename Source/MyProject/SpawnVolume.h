// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class MYPROJECT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
		class UBoxComponent* SpawnBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
		TSubclassOf<class ACritter> PawnToSpawn;	// Specify the type of class that can be the PawnToSpawn

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "SpawnVolume")
		FVector GetSpawnPoint();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SpawnVolume")
		void SpawnOurPawn(UClass* ToSpawn, const FVector& Location); // nealry everything we create (Actor, Character, Pawn etc..) inherits from UClass
};
